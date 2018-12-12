#pragma once

// fwd decl;
namespace redman { namespace backend { class Backend; }}
typedef redman::backend::Backend backend_t;

extern "C" {

// factory function to create Backend
backend_t* createBackend();

// factory function to delete Backend
void destroyBackend(backend_t*);

} // extern "C"
