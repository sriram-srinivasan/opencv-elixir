#include <opencv2/opencv.hpp>
#include "erl_nif.h"
#include "erl_cv_util.hpp"
#include <iostream>
using namespace std;

/*
 *  This code is a subset copy of exla_nif_util.cc from the Nx project
 */

ERL_NIF_TERM error(ErlNifEnv* env, const char* msg) {
  ERL_NIF_TERM atom = enif_make_atom(env, "error");
  ERL_NIF_TERM msg_term = enif_make_string(env, msg, ERL_NIF_LATIN1);
  return enif_make_tuple2(env, atom, msg_term);
}

ERL_NIF_TERM ok(ErlNifEnv* env, ERL_NIF_TERM term) {
  return enif_make_tuple2(env, ok(env), term);
}

ERL_NIF_TERM ok(ErlNifEnv* env) {
  return enif_make_atom(env, "ok");
}

 // Numeric types
ERL_NIF_TERM make(ErlNifEnv* env, int var) {
  return enif_make_int(env, var);
}

int get(ErlNifEnv* env, ERL_NIF_TERM term, int* var) {
  int value;
  if (!enif_get_int(env, term, &value)) return 0;
  return 1;
}


 // Standard types

int get(ErlNifEnv* env, ERL_NIF_TERM term, std::string &var) {
  unsigned len;
  int ret = enif_get_list_length(env, term, &len);

   if (!ret) {
    ErlNifBinary bin;
    ret = enif_inspect_binary(env, term, &bin);
    if (!ret) {
      return 0;
    }
    var = std::string((const char*)bin.data, bin.size);
    return ret;
  }

   var.resize(len+1);
  ret = enif_get_string(env, term, &*(var.begin()), var.size(), ERL_NIF_LATIN1);

   if (ret > 0) {
    var.resize(ret-1);
  } else if (ret == 0) {
    var.resize(0);
  } else {}

   return ret;
}

int get(ErlNifEnv* env, ERL_NIF_TERM term, bool* var) {
  int value;
  if (!enif_get_int(env, term, &value)) return 0;
  *var = static_cast<bool>(value);
  return 1;
}

ERL_NIF_TERM make(ErlNifEnv* env, ErlNifBinary var) {
  return enif_make_binary(env, &var);
}

ERL_NIF_TERM make(ErlNifEnv* env, std::string var) {
  return enif_make_string(env, var.c_str(), ERL_NIF_LATIN1);
}

ERL_NIF_TERM make(ErlNifEnv* env, const char* string) {
  return enif_make_string(env, string, ERL_NIF_LATIN1);
}

 // Atoms

int
get_atom(ErlNifEnv* env, ERL_NIF_TERM term, std::string &var) {
  unsigned atom_length;
  if (!enif_get_atom_length(env, term, &atom_length, ERL_NIF_LATIN1)) {
    return 0;
  }

   var.resize(atom_length+1);

   if (!enif_get_atom(env, term, &(*(var.begin())), var.size(), ERL_NIF_LATIN1)) return 0;

   var.resize(atom_length);

   return 1;
}

ERL_NIF_TERM 
atom(ErlNifEnv* env, const char* msg) {
  return enif_make_atom(env, msg);
}

 // Containers

int
get_tuple(ErlNifEnv* env, ERL_NIF_TERM tuple, std::vector<int> &var) {
  const ERL_NIF_TERM *terms;
  int length;
  if (!enif_get_tuple(env, tuple, &length, &terms)) return 0;
  var.reserve(length);

   for (int i = 0; i < length; i++) {
    int data;
    if (!get(env, terms[i], &data)) return 0;
    var.push_back(data);
  }
  return 1;
}

int 
get_list(ErlNifEnv* env,
             ERL_NIF_TERM list,
             std::vector<ErlNifBinary> &var) {
  unsigned int length;
  if (!enif_get_list_length(env, list, &length)) return 0;
  var.reserve(length);
  ERL_NIF_TERM head, tail;

   while (enif_get_list_cell(env, list, &head, &tail)) {
    ErlNifBinary elem;
    if (!get_binary(env, head, &elem)) return 0;
    var.push_back(elem);
    list = tail;
  }
  return 1;
}

int 
get_list(ErlNifEnv* env, ERL_NIF_TERM list, std::vector<int> &var) {
  unsigned int length;
  if (!enif_get_list_length(env, list, &length)) return 0;
  var.reserve(length);
  ERL_NIF_TERM head, tail;

   while (enif_get_list_cell(env, list, &head, &tail)) {
    int elem;
    if (!get(env, head, &elem)) return 0;
    var.push_back(elem);
    list = tail;
  }
  return 1;
}

int 
get_binary(ErlNifEnv* env, ERL_NIF_TERM term, ErlNifBinary* var) {
  return enif_inspect_binary(env, term, var);
}

ERL_NIF_TERM 
make_map(ErlNifEnv* env, std::map<std::string, int>& map) {
  ERL_NIF_TERM term = enif_make_new_map(env);
  std::map<std::string, int>::iterator itr;
  for (itr = map.begin(); itr != map.end(); ++itr) {
    ERL_NIF_TERM key = make(env, itr->first);
    ERL_NIF_TERM value = make(env, itr->second);
    enif_make_map_put(env, term, key, value, &term);
  }
  return term;
}

