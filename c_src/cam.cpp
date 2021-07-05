#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>

#include "erl_cv_util.hpp"
#include <erl_nif.h>
using namespace cv;
using namespace std;


ERL_NIF_TERM 
nif_imread(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    std::string path;
    if (!get(env, argv[0], path)) {
        return error(env, "Improper filename");
    }

    cv::Mat img = imread(path, IMREAD_COLOR);    
    if (img.empty()) {
        return error(env, "image not found");
    } else {
       return ok(env, make(env, img));
    }
}

ERL_NIF_TERM 
nif_imwrite(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    std::string path;
    if (!get(env, argv[0], path)) {
        return error(env, "Improper filename");
    }

    Mat *img;
    get<cv::Mat>(env, argv[1], img);
    if (img == NULL) {
        return error(env, "Image is NULL");
    } else if (img->empty()) {
        return error(env, "Image is empty");
    } else {
        return imwrite(path, *img) ? ok(env) : error(env, "Write unsuccessful");
    }
}
typedef struct _wrapper{
    VideoCapture *cam;
} wrapper;

/*
 * Open default camera 
 */
ERL_NIF_TERM
nif_open(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API

    VideoCapture cam = VideoCapture(deviceID, apiID);
    
    if(!cam.isOpened()){
        return error(env, "Unable to open camera");
    } else { 
        ERL_NIF_TERM term = make<cv::VideoCapture>(env, cam);
        return ok(env, term);
    }
}

/*
 * close camera
 */
ERL_NIF_TERM
nif_close(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    VideoCapture *cam = NULL;
    if (argc == 0 || ! get<cv::VideoCapture>(env, argv[0], cam)) {
        return error(env, "Cam.close <camera-ref>");
    } else {
        cam->release();
        return ok(env);
    }
}

ERL_NIF_TERM
nif_read(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    VideoCapture *cam = NULL;
    if (argc == 0 || ! get<cv::VideoCapture>(env, argv[0], cam)) {
        return error(env, "Cam.read <camera-ref>");
    } else {
        if (!cam->isOpened()) {
            return error(env, "Camera is not open. Call Cam.open again");
        } else {
            cv::Mat frame;
            if (!cam->read(frame) || frame.empty()) {
                return error(env, "Unable to retrieve frame");
            }
            return ok(env, make<cv::Mat>(env,frame));
        }
    }   
}

ERL_NIF_TERM
nif_qr_read(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
    std::string ret;
    QRCodeDetector qrd;
    if (enif_is_binary(env, argv[0])) {
        std::string path;
        get(env, argv[0], path);
        cv::Mat img = imread(path, IMREAD_COLOR);
        ret = qrd.detectAndDecode(img);
    } else {
        Mat *img;   
        if (!get<cv::Mat>(env, argv[0], img) || img->empty()) {
            return error(env, "Expected qr_read <image ref>|<image file path>");
        }
        ret = qrd.detectAndDecode(*img);    
    }
    return ok(env, make(env,ret)); 
}

static int load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info) {
    const char *mod = "Elixir.Cam";
    if (!open_resource<cv::Mat>(env, mod, "OpenCV_Mat")) {
        return -1;
    }
    if (!open_resource<cv::VideoCapture>(env, mod, "OpenCV_VideoCapture")) {
        return -1;
    }
    return 0;
}

ErlNifFunc nif_funcs[] = 
{
    {"imread",  1, nif_imread,  0},
    {"imwrite", 2, nif_imwrite, 0},
    {"open",    0, nif_open,    0},
    {"read",    1, nif_read,    0},
    {"qr_read", 1, nif_qr_read, 0},
    {"close",   1, nif_close,   0},
};

ERL_NIF_INIT(Elixir.Cam, nif_funcs, &load, nullptr, nullptr, nullptr);



