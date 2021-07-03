#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include "erl_cv_util.hpp"
#include <erl_nif.h>
using namespace cv;
using namespace std;


ERL_NIF_TERM nif_imread(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
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

ERL_NIF_TERM nif_imwrite(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
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



static int load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info) {
    const char *mod = "Elixir.Cam";
    if (!open_resource<cv::Mat>(env, mod, "OpenCV_Mat")) {
        return -1;
    }

    return 0;
}

ErlNifFunc nif_funcs[] = 
{
    {"imread",  1, nif_imread,0},
    {"imwrite", 2, nif_imwrite,0},
};

ERL_NIF_INIT(Elixir.Cam, nif_funcs, &load, nullptr, nullptr, nullptr);



