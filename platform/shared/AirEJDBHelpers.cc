/*
 Copyright (c) 2013 Justin Walsh, http://thejustinwalsh.com
 
 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "AirEJDBHelpers.h"
#include "ContextData.h"

namespace {
    static FREContext logContext = 0;
}

void _EnableLog(FREContext ctx, bool enabled)
{
    logContext = enabled ? ctx : 0;
}

void _Log(const char *format, ...)
{
    if (!logContext) return;
    
    static char message[1024 * 16];
    va_list argptr;
    va_start(argptr, format);
    vsprintf(message, format, argptr);
    va_end(argptr);
    
    FREDispatchStatusEventAsync(logContext, (const uint8_t*)"EJDBLog", (const uint8_t*)message);
}

std::string _GetDatabseIdFromArguments(FREObject argv[])
{
    FREObject dbContext = argv[0];
    return _GetDatabseId(dbContext);
}

std::string _GetDatabseId(FREObject dbContext)
{
    uint32_t idLength;
    const char *id;
    FREObject idProperty;
    FREGetObjectProperty(dbContext, (const uint8_t*)"id", &idProperty, 0);
    FREGetObjectAsUTF8(idProperty, &idLength, (const uint8_t**)&id);
    return std::string(id);
}

EJDB* _GetDatabseFromArguments(FREContext ctx, FREObject argv[])
{
    FREObject dbContext = argv[0];
    return _GetDatabse(ctx, dbContext);
}

EJDB* _GetDatabse(FREContext ctx, FREObject dbContext)
{
    ContextData *contextData = 0;
    FREGetContextNativeData(ctx, (void**)&contextData);
    
    std::string dbPathString = _GetDatabseId(dbContext);
    if (contextData->databases.find(dbPathString) == contextData->databases.end()) return 0;
    return contextData->databases[dbPathString];
}

// cachedrecords : Max number of cached records in shared memory segment. Default: 0
// records : Estimated number of records in this collection. Default: 65535.
// large : Specifies that the size of the database can be larger than 2GB. Default: false
// compressed : If true collection records will be compressed with DEFLATE compression. Default: false.
void _SetCollectionOptions(FREObject options, EJCOLLOPTS *opts)
{
    if (options == 0) return;
    
    FREObject cachedrecords;
    FREObject records;
    FREObject large;
    FREObject compressed;
    
    FREGetObjectProperty(options, (const uint8_t*)"cachedrecords", &cachedrecords, 0);
    FREGetObjectProperty(options, (const uint8_t*)"records", &records, 0);
    FREGetObjectProperty(options, (const uint8_t*)"large", &large, 0);
    FREGetObjectProperty(options, (const uint8_t*)"compressed", &compressed, 0);
    
    FREGetObjectAsInt32(cachedrecords, &opts->cachedrecords);
    FREGetObjectAsInt32(records, (int32_t*)&opts->records);

    uint32_t isLarge;
    FREGetObjectAsBool(large, &isLarge);
    opts->large = (isLarge != 0);

    uint32_t isCompressed;
    FREGetObjectAsBool(compressed, &isCompressed);
    opts->compressed = (isCompressed != 0);
    
    _Log("Collection Options: %d, %d, %d, %d", opts->cachedrecords, opts->records, opts->large ? 1 : 0, opts->compressed ? 1 : 0);
}

EJCOLL* _GetCollection(EJDB *ejdb, FREObject collection, FREObject options)
{
    const char *collectionName;
    uint32_t length;
    FREGetObjectAsUTF8(collection, &length, (const uint8_t**)&collectionName);
    
    EJCOLL *coll;
    if (options) {
        EJCOLLOPTS opts;
        _SetCollectionOptions(options, &opts);
        coll = ejdbcreatecoll(ejdb, collectionName, &opts);
    }
    else {
        coll = ejdbcreatecoll(ejdb, collectionName, 0);
    }
    
    return coll;
}

void _SetIndex(EJDB* ejdb, FREObject collection, FREObject path, int flags)
{
    EJCOLL *coll = _GetCollection(ejdb, collection, 0);
    if (!coll) {
        _Log("ejdbcreatecoll: %s", ejdberrmsg(ejdbecode(ejdb)));
        return;
    }
    
    const char *pathString;
    uint32_t length;
    FREGetObjectAsUTF8(collection, &length, (const uint8_t**)&pathString);
    
    ejdbsetindex(coll, pathString, flags);
}

bson* _FREObjectToBson(FREObject keys, FREObject values, FREObject types, bson *b, uint32_t depth, bool isQuery)
{
    depth++;
    if (depth > 1024) return b;
    
    uint32_t length = 0;
    FREGetArrayLength(keys, &length);
    
    if (!b) {
        b = bson_create();
        if (isQuery) {
            _Log("New BSON Query Object");
            bson_init_as_query(b);
        }
        else {
            _Log("New BSON Object");
            bson_init(b);
        }
    }
    
    FREObject key = 0;
    FREObject value = 0;
    FREObjectType valueType;
    FREObject type = 0;
    for (uint32_t i = 0; i < length; ++i) {
        FREGetArrayElementAt(keys, i, &key);
        FREGetArrayElementAt(values, i, &value);
        FREGetArrayElementAt(types, i, &type);
        FREGetObjectType(value, &valueType);
        
        const char *keyName = 0;
        uint32_t keyLength = 0;
        FREGetObjectAsUTF8(key, &keyLength, (const uint8_t**)&keyName);
        
        const char *typeName = 0;
        uint32_t typeLength = 0;
        FREGetObjectAsUTF8(type, &typeLength, (const uint8_t**)&typeName);
        
        _Log("Key %s is %s", keyName, typeName);
        
        switch (valueType) {
            case FRE_TYPE_VECTOR:
            case FRE_TYPE_ARRAY: {
                _Log("Key %s == \"Array or Vector\"", keyName);
                uint32_t valueLength = 0;
                FREGetArrayLength(value, &valueLength);
                
                bson_append_start_array(b, keyName);
                // TODO: Serialize the array
                bson_append_finish_array(b);
                break;
            }
                
            case FRE_TYPE_BOOLEAN: {
                _Log("Key %s == \"Boolean\"", keyName);
                uint32_t v;
                FREGetObjectAsBool(value, &v);
                bson_append_bool(b, keyName, (v != 0));
                break;
            }
                
            case FRE_TYPE_BITMAPDATA: {
                _Log("Key %s == \"BitmapData\"", keyName);
                // NOTE: Unsupported, should be serialized on the AS layer to a byte array anyhow
                break;
            }
                
            case FRE_TYPE_BYTEARRAY: {
                _Log("Key %s == \"ByteArray\"", keyName);
                FREByteArray bytes;
                FREAcquireByteArray(value, &bytes);
                bson_append_binary(b, keyName, BSON_BIN_BINARY, (const char*)bytes.bytes, bytes.length);
                FREReleaseByteArray(value);
                break;
            }
                
            case FRE_TYPE_NULL: {
                _Log("Key %s == \"null\"", keyName);
                bson_append_null(b, keyName);
                break;
            }
                
            case FRE_TYPE_NUMBER: {
                if (!strcmp(typeName, "int") || !strcmp(typeName, "uint")) {
                    _Log("Key %s == \"int\"", keyName);
                    int v;
                    FREGetObjectAsInt32(value, (int32_t*)&v);
                    bson_append_int(b, keyName, v);
                }
                else if (!strcmp(typeName, "Date")) {
                    _Log("Key %s == \"Date\"", keyName);
                    double v;
                    FREGetObjectAsDouble(value, &v);
                    bson_append_date(b, keyName, (bson_date_t)v);
                }
                else {
                    _Log("Key %s == \"Number\"", keyName);
                    double v;
                    FREGetObjectAsDouble(value, &v);
                    bson_append_double(b, keyName, v);
                }
                break;
            }
                
            case FRE_TYPE_OBJECT: {
                if (!strcmp(typeName, "RegExp")) {
                    // Regular Expression
                    FREObject pattern;
                    FREGetObjectProperty(value, (const uint8_t*)"pattern", &pattern, 0);
                    FREObject options;
                    FREGetObjectProperty(value, (const uint8_t*)"options", &options, 0);
                    
                    uint32_t length = 0;
                    const char *strPattern;
                    const char *strOptions;
                    FREGetObjectAsUTF8(pattern, &length, (const uint8_t**)&strPattern);
                    FREGetObjectAsUTF8(options, &length, (const uint8_t**)&strOptions);
                    
                    bson_append_regex(b, keyName, strPattern, strOptions);
                }
                else {
                    // Object or Array
                    FREObject subKeys;
                    FREGetObjectProperty(value, (const uint8_t*)"keys", &subKeys, 0);
                    FREObject subValues;
                    FREGetObjectProperty(value, (const uint8_t*)"values", &subValues, 0);
                    FREObject subTypes;
                    FREGetObjectProperty(value, (const uint8_t*)"types", &subTypes, 0);
                    
                    bool isObject = !strcmp(typeName, "Object");
                    _Log("Key %s == \"%s\"", keyName, isObject ? "Object" : "Array");
                    isObject ? bson_append_start_object(b, keyName) : bson_append_start_array(b, keyName);
                    _FREObjectToBson(subKeys, subValues, subTypes, b, depth);
                    isObject ? bson_append_finish_object(b) : bson_append_finish_array(b);
                }
                break;
            }
                
            case FRE_TYPE_STRING: {
                _Log("Key %s == \"String\"", keyName);
                uint32_t length = 0;
                const char *v;
                FREGetObjectAsUTF8(value, &length, (const uint8_t**)&v);
                
                // If this is the top level _id then set it
                if (depth == 1 && !strcmp(keyName, JDBIDKEYNAME)) {
                    bson_oid_t oid;
                    if (length == 0) {
                        bson_oid_gen(&oid); // Create an oid for this object
                    }
                    else {
                        bson_oid_from_string(&oid, v); // Use the oid provided from the user for this object
                    }
                    bson_append_oid(b, keyName, &oid);
                }
                else {
                    bson_append_string_n(b, keyName, v, length);
                }
                break;
            }
                
            default: {
                break;
            }
        }
    }
    
    return b;
}

void _SetObjectValue(bson_type type, const char *key, uint32_t knum, FREObject *object, FREObject *value)
{
    if (type == BSON_ARRAY) {
        FRESetArrayElementAt(*object, knum, *value);
    } else {
        FRESetObjectProperty(*object, (const uint8_t*)key, *value, 0);
    }
}

void _FRENewObjectFromBSON(bson_iterator *it, bson_type type, FREObject *value)
{
    switch (type) {
            
        case BSON_OID: {
            char xoid[25];
            bson_oid_to_string(bson_iterator_oid(it), xoid);
            
            FRENewObjectFromUTF8(25, (const uint8_t*)xoid, value);
            break;
        }
            
        case BSON_STRING:
        case BSON_SYMBOL: {
            const char *string = bson_iterator_string(it);
            uint32_t length = bson_iterator_string_len(it);
            
            FRENewObjectFromUTF8(length, (const uint8_t*)string, value);
            break;
        }
            
        case BSON_REGEX: {
            FREObject constructorArgs[2];
            const char *strPattern = bson_iterator_regex(it);
            const char *strOptions = bson_iterator_regex_opts(it);
            FRENewObjectFromUTF8(strlen(strPattern), (const uint8_t*)strPattern, &constructorArgs[0]);
            FRENewObjectFromUTF8(strlen(strOptions), (const uint8_t*)strOptions, &constructorArgs[1]);
            FRENewObject((const uint8_t*)"RegExp", 2, constructorArgs, value, 0);
            break;
        }
            
        case BSON_INT: {
            int integer = bson_iterator_int_raw(it);
            FRENewObjectFromInt32(int32_t(integer), value);
            break;
        }
            
        case BSON_LONG: {
            int64_t number = bson_iterator_long_raw(it);
            FRENewObjectFromDouble((double)number, value);
            break;
        }
            
        case BSON_DOUBLE: {
            double number = bson_iterator_double_raw(it);
            FRENewObjectFromDouble((double)number, value);
            break;
        }
            
        case BSON_BOOL: {
            int32_t boolean = bson_iterator_bool_raw(it);
            FRENewObjectFromBool(boolean, value);
            break;
        }
            
        case BSON_OBJECT:
        case BSON_ARRAY: {
            bson_iterator sub_it;
            bson_iterator_subiterator(it, &sub_it);
            
            _BSONToFREObject(&sub_it, type, value);
            break;
        }
            
        case BSON_DATE: {
            bson_date_t date = bson_iterator_date(it);
            FREObject time;
            FRENewObjectFromDouble((double)date, &time);
            
            FRENewObject((const uint8_t*)"Date", 0, 0, value, 0);
            FREObject argv[1] = {time};
            FREObject result;
            FRECallObjectMethod(*value, (const uint8_t*)"setTime", 1, argv, &result, 0);
            break;
        }
            
        case BSON_BINDATA: {
            FREObject length;
            const char *data = bson_iterator_bin_data(it);
            uint32_t dataLength = bson_iterator_bin_len(it);
            
            FRENewObject((const uint8_t*)"flash.utils::ByteArray", 0, 0, value, 0);
            FRENewObjectFromUint32(dataLength, &length);
            FRESetObjectProperty(*value, (const uint8_t*)"length", length, 0);
            
            FREByteArray bytes;
            FREAcquireByteArray(*value, &bytes);
            memcpy(bytes.bytes, data, bytes.length);
            FREReleaseByteArray(*value);
            break;
        }
            
        case BSON_NULL:
        case BSON_UNDEFINED:
        default: {
            *value = 0;
            break;
        }
    }
}

void _BSONToFREObject(bson_iterator *it, bson_type type, FREObject *object)
{
    uint32_t knum = 0;
    if (type == BSON_ARRAY) {
        _Log("New Array");
        FRENewObject((const uint8_t*)"Array", 0, 0, object, 0);
    }
    else {
        _Log("New Object");
        FRENewObject((const uint8_t*)"Object", 0, 0, object, 0);
    }
    
    bson_type bt;
    if (type == BSON_ARRAY) {
        bson_iterator counter = *it;
        uint32_t iterator_count = 0;
        while ((bson_iterator_next(&counter)) != BSON_EOO) iterator_count++;
        FRESetArrayLength(*object, iterator_count);
    }
    
    while ((bt = bson_iterator_next(it)) != BSON_EOO) {
        const char *key = bson_iterator_key(it);
        if (type == BSON_ARRAY) knum = (uint32_t) tcatoi(key);
        
        if (type == BSON_ARRAY)  _Log("Found Key: %d", knum);
        if (type == BSON_OBJECT) _Log("Found Key: %s", key);
        
        FREObject value;
        _FRENewObjectFromBSON(it, bt, &value);
        _SetObjectValue(type, key, knum, object, &value);
    }
}

