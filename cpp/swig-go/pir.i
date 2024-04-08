%module pir

%{
#include "pir_client.h"
#include "pir_server.h"
#include "Faker.h"
%}
%include "Faker.h"
%include "pir_client.h"
%include "pir_server.h"
%include "std_string.i"
%include "std_vector.i"
%include "stdint.i"
%include "typemaps.i"

%template(VectorUInt8) std::vector<uint8_t>;
%template(ArrayofVectorUInt8) std::vector<std::vector<uint8_t>>;
