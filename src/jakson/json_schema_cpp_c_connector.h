#ifndef JSON_SCHEMA_CPP_C_CONNECTOR_H 
#define JSON_SCHEMA_CPP_C_CONNECTOR_H 

#ifdef __cplusplus
extern "C" {
#endif

void json_validator_set_root_schema(const json &);
void json_validator_validate(const json &) const;

#ifdef __cplusplus
}
#endif


#endif
