#ifndef ERL_CV_UTIL_H
#define ERL_CV_UTIL_H
#include <opencv2/opencv.hpp>

#include "erl_nif.h"
#include <cstdint>
#include <complex>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <map>

/*
 *  This is a subset of exla_nif_util.h from the Nx project.
 */



// Helper for returning `{:error, msg}` from NIF.
ERL_NIF_TERM error(ErlNifEnv* env, const char* msg);

// Helper for returning `{:ok, term}` from NIF.
ERL_NIF_TERM ok(ErlNifEnv* env, ERL_NIF_TERM term);

// Helper for returning `:ok` from NIF.
ERL_NIF_TERM ok(ErlNifEnv* env);


// get retrieves the C++ value from term and returns it in the last param by reference
// The return value is 1 for success and 0 for failure.
int get(ErlNifEnv* env, ERL_NIF_TERM term, int* var);
int get(ErlNifEnv* env, ERL_NIF_TERM term, std::string &var);
int get(ErlNifEnv* env, ERL_NIF_TERM term, bool* var);
int get(ErlNifEnv* env, ERL_NIF_TERM term, cv::Mat* var);

ERL_NIF_TERM make(ErlNifEnv* env, int var);
ERL_NIF_TERM make(ErlNifEnv* env, std::string var);
ERL_NIF_TERM make(ErlNifEnv* env, ErlNifBinary var);
ERL_NIF_TERM make(ErlNifEnv* env, const char* string);

// Atoms
//
// We have to be explicit in naming these functions because
// their signatures are the same for retrieving/returning
// regular strings.

int get_atom(ErlNifEnv* env, ERL_NIF_TERM term, std::string* var);

ERL_NIF_TERM atom(ErlNifEnv* env, const char* status);

// Template struct for resources. The struct lets us use templates
// to store and retrieve open resources later on. This implementation
// is the same as the approach taken in the goertzenator/nifpp
// C++11 wrapper around the Erlang NIF API.
template <typename T>
struct resource_object {
  static ErlNifResourceType *type;
};
template<typename T> ErlNifResourceType* resource_object<T>::type = 0;

// Default destructor passed when opening a resource. The default
// behavior is to invoke the underlying objects destructor and
// set the resource pointer to NULL.
template <typename T>
void default_dtor(ErlNifEnv* env, void * obj) {
  T* resource = reinterpret_cast<T*>(obj);
  resource->~T();
  resource = nullptr;
}

// Opens a resource for the given template type T. If no
// destructor is given, uses the default destructor defined
// above.
template <typename T>
int open_resource(ErlNifEnv* env,
                  const char* mod,
                  const char* name,
                  ErlNifResourceDtor* dtor = nullptr) {
  if (dtor == nullptr) {
    dtor = &default_dtor<T>;
  }
  ErlNifResourceType *type;
  ErlNifResourceFlags flags = ErlNifResourceFlags(ERL_NIF_RT_CREATE|ERL_NIF_RT_TAKEOVER);
  type = enif_open_resource_type(env, mod, name, dtor, flags, NULL);
  if (type == NULL) {
    resource_object<T>::type = 0;
    return -1;
  } else {
    resource_object<T>::type = type;
  }
  return 1;
}

// Returns a resource of the given template type T.
template <typename T>
ERL_NIF_TERM get(ErlNifEnv* env, ERL_NIF_TERM term, T* &var) {
  return enif_get_resource(env, term,
                           resource_object<T>::type,
                           reinterpret_cast<void**>(&var));
}

// Creates a reference to the given resource of type T. We
// use the move constructor by default because some XLA
// objects delete the copy-constructor. The move is intended
// to represent a transfer of ownership of the object to
// the VM.
template <typename T>
ERL_NIF_TERM make(ErlNifEnv* env, T &var) {
  void* ptr = enif_alloc_resource(resource_object<T>::type, sizeof(T));
  new(ptr) T(std::move(var));
  ERL_NIF_TERM ret = enif_make_resource(env, ptr);
  enif_release_resource(ptr);
  return ret;
}

// Containers
//
// Both tuples and lists are treated as vectors, but extracting
// terms from both is slightly different, so we have to be
// explicit in the naming convention in order to differentiate.
//
// We also support reading resources into vectors from both tuples
// and lists. Once again, implementation is slightly different
// for resources, so we need to be explicit.
//
// Similar to standard types, we only define implementations for
// types used.

int get_tuple(ErlNifEnv* env,
              ERL_NIF_TERM tuple,
              std::vector<int> &var);

template <typename T>
int get_tuple(ErlNifEnv* env, ERL_NIF_TERM tuple, std::vector<T> &var) {
  const ERL_NIF_TERM* terms;
  int length;
  if (!enif_get_tuple(env, tuple, &length, &terms)) return 0;
  var.reserve(length);

  for (int i=0; i < length; i++) {
    T* elem;
    if (!get<T>(env, terms[i], elem)) return 0;
    var.push_back(*elem);
  }
  return 1;
}

int get_list(ErlNifEnv* env,
             ERL_NIF_TERM list,
             std::vector<int> &var);
int get_list(ErlNifEnv* env,
             ERL_NIF_TERM list,
             std::vector<ErlNifBinary> &var);

template <typename T>
int get_list(ErlNifEnv* env, ERL_NIF_TERM list, std::vector<T*> &var) {
  unsigned int length;
  if (!enif_get_list_length(env, list, &length)) return 0;
  var.reserve(length);
  ERL_NIF_TERM head, tail;

  while (enif_get_list_cell(env, list, &head, &tail)) {
    T* elem;
    if (!get<T>(env, head, elem)) return 0;
    var.push_back(elem);
    list = tail;
  }
  return 1;
}

template <typename T>
int get_list(ErlNifEnv* env, ERL_NIF_TERM list, std::vector<T> &var) {
  unsigned int length;
  if (!enif_get_list_length(env, list, &length)) return 0;
  var.reserve(length);
  ERL_NIF_TERM head, tail;

  while (enif_get_list_cell(env, list, &head, &tail)) {
    T* elem;
    if (!get<T>(env, head, elem)) return 0;
    var.push_back(*elem);
    list = tail;
  }
  return 1;
}

int get_binary(ErlNifEnv* env, ERL_NIF_TERM term, ErlNifBinary* var);

ERL_NIF_TERM make_map(ErlNifEnv* env, std::map<std::string, int>& map);


// OpenCV types





#endif