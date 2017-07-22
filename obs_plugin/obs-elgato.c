#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <obs-module.h>
#include <util/darray.h>
#include <util/threading.h>
#include <util/platform.h>

#define SOCKET_PATH "/tmp/elgato_raw_frames"

#define blog(log_level, format, ...) \
    blog(log_level, "[elgato_source: '%s'] " format, obs_source_get_name(s->source), ##__VA_ARGS__)
#define debug(format, ...) \
    blog(LOG_DEBUG, format, ##__VA_ARGS__)
#define info(format, ...) \
    blog(LOG_INFO, format, ##__VA_ARGS__)
#define warn(format, ...) \
    blog(LOG_WARNING, format, ##__VA_ARGS__)

OBS_DECLARE_MODULE()

struct elgato_source {
    obs_source_t* source;
    DARRAY(uint8_t) buffer;
    bool done;
    pthread_t thread;
};

static void* run_thread(void* data);
static int connect_to_frame_server(struct elgato_source* s);
static bool read_frame(struct elgato_source* s, int fd);
static bool read_bytes(struct elgato_source* s, int fd, void* buf, size_t len);

static const char *get_name(void* typeData) {
    UNUSED_PARAMETER(typeData);
    return "Elgato HD60S";
}

static void* create(obs_data_t *settings, obs_source_t *source) {
    UNUSED_PARAMETER(settings);

    struct elgato_source* s = bzalloc(sizeof(struct elgato_source));
    s->source = source;
    s->done = false;
    da_init(s->buffer);
    pthread_create(&s->thread, NULL, run_thread, s);

    return s;
}

static void destroy(void* data) {
    struct elgato_source* s = (struct elgato_source*)data;

    s->done = true;
    pthread_join(s->thread, NULL);
    da_free(s->buffer);
    bfree(s);
}

static void* run_thread(void* data) {
    struct elgato_source* s = (struct elgato_source*)data;

    int fd = -1;
    while (!s->done) {
        if (fd < 0) {
            fd = connect_to_frame_server(s);
            if (fd < 0) {
                sleep(1);
            }
        }

        if (fd >= 0) {
            if (!read_frame(s, fd)) {
                close(fd);
                fd = -1;
            }
        }
    }

    return NULL;
}

static int connect_to_frame_server(struct elgato_source* s) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket()");
        warn("Unable to create frame-client socket");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect()");
        warn("Unable to connect to frame-server");
        return -1;
    }

    info("Connected to frame-server");
    return fd;
}

static bool read_frame(struct elgato_source* s, int fd) {
    int width, height, colorspace, data_length;
    if (!read_bytes(s, fd, &width, sizeof(width))) goto error;
    if (!read_bytes(s, fd, &height, sizeof(height))) goto error;
    if (!read_bytes(s, fd, &colorspace, sizeof(colorspace))) goto error;
    if (!read_bytes(s, fd, &data_length, sizeof(data_length))) goto error;
    
    da_reserve(s->buffer, data_length);
    uint8_t* data = s->buffer.array;
    if (!read_bytes(s, fd, data, data_length)) goto error;

    debug("Received %dx%d frame", width, height);

    struct obs_source_frame frame = {
        .format      = VIDEO_FORMAT_I420,
        .width       = width,
        .height      = height,
        .data[0]     = data,
        .data[1]     = data + (width * height),
        .data[2]     = data + (width * height) + (width * height) / 4,
        .linesize[0] = width,
        .linesize[1] = width / 2,
        .linesize[2] = width / 2,
        .timestamp   = os_gettime_ns()
    };
    video_format_get_parameters(VIDEO_CS_DEFAULT, VIDEO_RANGE_PARTIAL,
        frame.color_matrix, frame.color_range_min, frame.color_range_max);
    obs_source_output_video(s->source, &frame);

    return true;

error:
    return false;
}

static bool read_bytes(struct elgato_source* s, int fd, void* buf, size_t len) {
    size_t recv_len = 0;
    while (recv_len < len) {
        ssize_t l = recv(fd, (uint8_t*)buf + recv_len, len - recv_len, 0);
        if (l < 0) {
            perror("recv()");
            warn("Unable to receive data from frame-server");
            return false;
        } else if (l == 0) {
            warn("Connection-lost with frame-server");
            return false;
        }
        recv_len += l;
    }
    return true;
}

static struct obs_source_info elgatoSourceInfo = {
    .id           = "elgato_source",
    .type         = OBS_SOURCE_TYPE_INPUT,
    .output_flags = OBS_SOURCE_ASYNC_VIDEO | OBS_SOURCE_DO_NOT_DUPLICATE,
    .get_name     = get_name,
    .create       = create,
    .destroy      = destroy
};

bool obs_module_load(void) {
    obs_register_source(&elgatoSourceInfo);
    return true;
}
