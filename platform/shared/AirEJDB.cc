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

#include <cstdio>
#include <cstdlib>
#include <locale>

#include "AirEJDB.h"
#include "AirEJDBHelpers.h"
#include "ContextData.h"
#include "CursorData.h"

#include "tcejdb/ejdb.h"
#include "tcejdb/bson.h"

namespace {
    uint32_t cursor_id = 0;
}


#pragma mark -
#pragma mark Context


void AirEJDBExtInitializer(void** extDataToSet, FREContextInitializer* ctxInitializerToSet, FREContextFinalizer* ctxFinalizerToSet)
{
    setlocale(LC_ALL, "en_US.UTF-8");
    
    *extDataToSet = NULL;
    *ctxInitializerToSet = &AirEJDBContextInitializer;
    *ctxFinalizerToSet = &AirEJDBContextFinalizer;
}

void AirEJDBExtFinalizer(void* extData)
{
    
}

void AirEJDBContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctionsToSet, const FRENamedFunction** functionsToSet)
{    
    ContextData *contextData = new ContextData(ctx);
    FRESetContextNativeData(ctx, contextData);

    #define STR(X) (const uint8_t*)(X)
    FRENamedFunction FunctionMap[] = {
        
        // Database Functions
        { STR("db.open"), 0, &AirEJDBOpen },
        { STR("db.close"), 0, &AirEJDBClose },
        { STR("db.isOpen"), 0, &AirEJDBIsOpen },
        { STR("db.ensureCollection"), 0, &AirEJDBEnsureCollection },
        { STR("db.dropCollection"), 0, &AirEJDBDropCollection },
        { STR("db.save"), 0, &AirEJDBSave },
        { STR("db.load"), 0, &AirEJDBLoad },
        { STR("db.remove"), 0, &AirEJDBRemove },
        { STR("db.find"), 0, &AirEJDBFind },
        { STR("db.dropIndexes"), 0, &AirEJDBDropIndexes },
        { STR("db.optimizeIndexes"), 0, &AirEJDBOptimizeIndexes },
        { STR("db.ensureStringIndex"), 0, &AirEJDBEnsureStringIndex },
        { STR("db.ensureIStringIndex"), 0, &AirEJDBEnsureIStringIndex },
        { STR("db.ensureNumberIndex"), 0, &AirEJDBEnsureNumberIndex },
        { STR("db.ensureArrayIndex"), 0, &AirEJDBEnsureArrayIndex },
        { STR("db.rebuildStringIndex"), 0, &AirEJDBRebuildStringIndex },
        { STR("db.rebuildIStringIndex"), 0, &AirEJDBRebuildIStringIndex },
        { STR("db.rebuildNumberIndex"), 0, &AirEJDBRebuildNumberIndex },
        { STR("db.rebuildArrayIndex"), 0, &AirEJDBRebuildArrayIndex },
        { STR("db.dropStringIndex"), 0, &AirEJDBDropStringIndex },
        { STR("db.dropIStringIndex"), 0, &AirEJDBDropIStringIndex },
        { STR("db.dropNumberIndex"), 0, &AirEJDBDropNumberIndex },
        { STR("db.dropArrayIndex"), 0, &AirEJDBDropArrayIndex },
        
        // Cursor Functions
        { STR("cursor.close"), 0, &AirEJDBCursorClose },
        { STR("cursor.field"), 0, &AirEJDBCursorGetField },
        { STR("cursor.object"), 0, &AirEJDBCursorGetObject },
        
        // Debugging Functions
        { STR("debug.logging"), 0, &AirEJDBDebugLogging },
    };
    #undef STR
    
    *numFunctionsToSet = sizeof(FunctionMap) / sizeof(FRENamedFunction);
    FRENamedFunction* func = (FRENamedFunction*) malloc(sizeof(FRENamedFunction) * (*numFunctionsToSet));
    memcpy(func, &FunctionMap, sizeof(FRENamedFunction) * (*numFunctionsToSet));
    *functionsToSet = func;
}

void AirEJDBContextFinalizer(FREContext ctx)
{
    ContextData *contextData = 0;
    FREGetContextNativeData(ctx, (void**)&contextData);
    if (contextData == 0) return;
    
    DatabaseMap::iterator dbIterator;
    for (dbIterator = contextData->databases.begin(); dbIterator != contextData->databases.end(); ++dbIterator) {
        EJDB* ejdb = dbIterator->second;
        ejdbclose(ejdb);
        
        IdList::iterator cIterator;
        IdList cursorList = contextData->openCursors[dbIterator->first];
        for (cIterator = cursorList.begin(); cIterator != cursorList.end(); ++cIterator) {
            tclistdel(contextData->cursors[*cIterator].results);
            contextData->cursors.erase(*cIterator);
        }
    }
    contextData->databases.clear();
    
    delete contextData;
    FRESetContextNativeData(ctx, 0);
}

// (enabled:Boolean)
FREObject AirEJDBDebugLogging(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    FREObject enabled = argv[0];
    uint32_t logEnabled = 0;
    FREGetObjectAsBool(enabled, &logEnabled);
    _EnableLog(ctx, logEnabled != 0);
    
    return 0;
}


#pragma mark -
#pragma mark Database


// (database:String, mode:uint, async:Boolean)
FREObject AirEJDBOpen(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    ContextData *contextData = 0;
    FREGetContextNativeData(ctx, (void**)&contextData);
    
    uint32_t dbPathLength;
    const char *dbPath;
    FREGetObjectAsUTF8(argv[0], &dbPathLength, (const uint8_t**)&dbPath);
    
    uint32_t dbMode;
    FREGetObjectAsUint32(argv[1], &dbMode);
    
    EJDB *ejdb = ejdbnew();
    bool opened = ejdbopen(ejdb, dbPath, dbMode);
    
    FREObject dbContext;
    FRENewObject((const uint8_t*)"Object", 0, NULL, &dbContext, 0);

    if (opened) {
        std::string dbPathString(dbPath);
        contextData->databases[dbPathString] = ejdb;
        
        FREObject id;
        FRENewObjectFromUTF8(dbPathLength, (const uint8_t*)dbPath, &id);
        FRESetObjectProperty(dbContext, (const uint8_t*)"id", id, 0);
    }
    else {
        FREObject error;
        const char *errorString = ejdberrmsg(ejdbecode(ejdb));
        FRENewObjectFromUTF8(strlen(errorString), (const uint8_t*)errorString, &error);
        FRESetObjectProperty(dbContext, (const uint8_t*)"error", error, 0);
        ejdbdel(ejdb);
    }
    
    return dbContext;
}

// (dbContext:Object)
FREObject AirEJDBClose(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    ContextData *contextData = 0;
    FREGetContextNativeData(ctx, (void**)&contextData);
    std::string dbStringPath = _GetDatabseIdFromArguments(argv);

    EJDB* ejdb = contextData->databases[dbStringPath];
    ejdbclose(ejdb);
    
    IdList::iterator cIterator;
    IdList cursorList = contextData->openCursors[dbStringPath];
    for (cIterator = cursorList.begin(); cIterator != cursorList.end(); ++cIterator) {
        if (contextData->cursors.find(*cIterator) != contextData->cursors.end()) {
            CursorData cursorData = contextData->cursors[*cIterator];
            contextData->cursors.erase(*cIterator);
            
            ejdbquerydel(cursorData.query); cursorData.query = 0;
            tclistdel(cursorData.results); cursorData.results = 0;
        }
    }
    
    contextData->databases.erase(dbStringPath);
    
    return 0;
}

// (dbContext:Object)
FREObject AirEJDBIsOpen(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    ContextData *contextData = 0;
    FREGetContextNativeData(ctx, (void**)&contextData);
    std::string dbStringPath = _GetDatabseIdFromArguments(argv);
    
    FREObject isOpen;
    FRENewObjectFromBool((contextData->databases.find(dbStringPath) != contextData->databases.end()) ? 1 : 0, &isOpen);
    
    return isOpen;
}

// (dbContext:Object, collection:String, options:Object)
FREObject AirEJDBEnsureCollection(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject options = argv[2];
    
    const char *collectionName;
    uint32_t length;
    FREGetObjectAsUTF8(collection, &length, (const uint8_t**)&collectionName);
    
    EJCOLLOPTS opts;
    _SetCollectionOptions(options, &opts);
    ejdbcreatecoll(ejdb, collectionName, &opts);
    
    return 0;
}

// (dbContext:Object, collection:String, prune:Boolean)
FREObject AirEJDBDropCollection(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject prune = argv[2];
    
    const char *collectionName;
    uint32_t length;
    FREGetObjectAsUTF8(collection, &length, (const uint8_t**)&collectionName);
    
    uint32_t pruneValue;
    FREGetObjectAsBool(prune, &pruneValue);
    
    bool success = ejdbrmcoll(ejdb, collectionName, (pruneValue != 0));
    if (!success) {
        _Log("ejdbrmcoll: %s", ejdberrmsg(ejdbecode(ejdb)));
        return 0;
    }
    
    return 0;
}

// (dbContext:Object, collection:String, keys:Array, values:Array)
FREObject AirEJDBSave(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject keys = argv[2];
    FREObject values = argv[3];
    FREObject types = argv[4];
    FREObject defaultOptions = argv[5];
    
    EJCOLL *coll = _GetCollection(ejdb, collection, defaultOptions);
    if (!coll) {
        _Log("ejdbcreatecoll: %s", ejdberrmsg(ejdbecode(ejdb)));
        return 0;
    }
    
    uint32_t length = 0;
    FREGetArrayLength(keys, &length);
    if (length == 0) return 0;
    
    FREObject oids;
    FRENewObject((const uint8_t*)"Array", 0, 0, &oids, 0);
    FRESetArrayLength(oids, length);
    
    _Log("Saving %d key(s)...", length);
    for (uint32_t i = 0; i < length; ++i) {
        FREObject oidValue = 0;
        
        // Get the keys, values, and types for the current object
        FREObject oKeys;
        FREGetArrayElementAt(keys, i, &oKeys);
        FREObject oValues;
        FREGetArrayElementAt(values, i, &oValues);
        FREObject oTypes;
        FREGetArrayElementAt(types, i, &oTypes);
        
        FREObjectType oKeysType;
        FREGetObjectType(oKeys, &oKeysType);
        if (oKeysType == FRE_TYPE_ARRAY) {
            // Create the object
            bson *b = _FREObjectToBson(oKeys, oValues, oTypes);
            bson_finish(b);
            
            // Store the object
            bson_oid_t oid;
            
            bool success = ejdbsavebson(coll, b, &oid);
            if (success) {
                char oidString[25];
                uint32_t oidStringLength = 0;
                oidStringLength = strlen(oidString);
                bson_oid_to_string(&oid, oidString);
                FRENewObjectFromUTF8(25, (const uint8_t*)oidString, &oidValue);
                _Log("New OID: %s", oidString);
            }
            else {
                _Log("Invalid OID, record not saved");
                _Log(ejdberrmsg(ejdbecode(ejdb)));
            }
            
            // Destroy the object from memory
            bson_destroy(b);
        }
        
        // Put the new oid back into the return array
        FRESetArrayElementAt(oids, i, oidValue);
    }
    
    return oids;
}

// (dbContext:Object, collection:String, oid:String)
FREObject AirEJDBLoad(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject oid = argv[2];
    
    EJCOLL *col = _GetCollection(ejdb, collection, 0);
    if (!col) {
        _Log(ejdberrmsg(ejdbecode(ejdb)));
        return 0;
    }
    
    const char *oidValue;
    uint32_t length;
    FREGetObjectAsUTF8(oid, &length, (const uint8_t**)&oidValue);
    bson_oid_t bsonOID;
    bson_oid_from_string(&bsonOID, oidValue);    
    bson *thisBson = ejdbloadbson(col, &bsonOID);
    if (!thisBson) {
        _Log(ejdberrmsg(ejdbecode(ejdb)));
        return 0;
    }
    
    bson_iterator it;
    bson_iterator_init(&it, thisBson);
    
    FREObject object;
    _BSONToFREObject(&it, BSON_OBJECT, &object);
    
    bson_destroy(thisBson);
    
    return object;
}

// (dbContext:Object, collection:String, oid:String)
FREObject AirEJDBRemove(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB *ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject oid = argv[3];
    
    EJCOLL *coll = _GetCollection(ejdb, collection, 0);
    if (!coll) {
        _Log("ejdbcreatecoll: %s", ejdberrmsg(ejdbecode(ejdb)));
        return 0;
    }
    
    const char *oidValue;
    uint32_t length;
    FREGetObjectAsUTF8(oid, &length, (const uint8_t**)&oidValue);
    bson_oid_t bsonOID;
    bson_oid_from_string(&bsonOID, oidValue);
    
    bool success = ejdbrmbson(coll, &bsonOID);
    if (!success) _Log("ejdbrmbson: %s", ejdberrmsg(ejdbecode(ejdb)));
    
    return 0;
}

FREObject AirEJDBFind(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    ContextData *contextData = 0;
    FREGetContextNativeData(ctx, (void**)&contextData);
    
    std::string dbStringPath = _GetDatabseIdFromArguments(argv);
    if (contextData->databases.find(dbStringPath) == contextData->databases.end()) { _Log("Database is closed"); return 0; }
    EJDB *ejdb = contextData->databases[dbStringPath];

    FREObject collection = argv[1];
    
    FREObject queryKeys = argv[2];
    FREObject queryValues = argv[3];
    FREObject queryTypes = argv[4];
    
    FREObject orArrayKeys = argv[5];
    FREObject orArrayValues = argv[6];
    FREObject orArrayTypes = argv[7];
    
    FREObject hintsKeys = argv[8];
    FREObject hintsValues = argv[9];
    FREObject hintsTypes = argv[10];

    EJCOLL *coll = _GetCollection(ejdb, collection, 0);
    if (!coll) {
        _Log("ejdbcreatecoll: %s", ejdberrmsg(ejdbecode(ejdb)));
        return 0;
    }
    
    bson *query = _FREObjectToBson(queryKeys, queryValues, queryTypes, 0, 0, true);
    bson_finish(query);
 
    bson *hints = _FREObjectToBson(hintsKeys, hintsValues, hintsTypes, 0, 0, true);
    bson_finish(hints);
    
    bson *orArray = 0;
    uint32_t orArrayLength = 0;
    FREGetArrayLength(orArrayKeys, &orArrayLength);
    if (orArrayLength > 0) {
        orArray = _FREObjectToBson(orArrayKeys, orArrayValues, orArrayTypes, 0, 0, true);
        bson_finish(orArray);
    }
    
    EJQ* q = ejdbcreatequery(ejdb, query, orArray, orArrayLength, hints);
    if (!q) {
        _Log("ejdbcreatequery: %s", ejdberrmsg(ejdbecode(ejdb)));
        return 0;
    }
    
    // Get flags from hints
    uint32_t flags = 0;
    bson_iterator it;
    bson_iterator_init(&it, hints);
    bson_type it_type = bson_find_fieldpath_value("$onlycount", &it);
    if (it_type == BSON_BOOL && bson_iterator_bool(&it)) flags = JBQRYCOUNT;
    
    // Execute query
    uint32_t count = 0;
    TCXSTR *log = tcxstrnew();
    TCLIST *cursor = ejdbqryexecute(coll, q, &count, flags, log);
        
    // Store the cursor in our map
    uint32_t current_id;
    while (contextData->cursors.find(current_id = cursor_id++) != contextData->cursors.end());
    contextData->cursors[current_id] = CursorData(q, cursor);
    contextData->openCursors[dbStringPath].push_back(current_id);
    
    // Return the cursor data
    FREObject cursorContext;
    FRENewObject((const uint8_t*)"Object", 0, NULL, &cursorContext, 0);
    
    FREObject id;
    FRENewObjectFromUint32(current_id, &id);
    FRESetObjectProperty(cursorContext, (const uint8_t*)"id", id, 0);
    
    FREObject valid;
    FRENewObjectFromBool(1, &valid);
    FRESetObjectProperty(cursorContext, (const uint8_t*)"valid", valid, 0);
    
    FREObject pos;
    FRENewObjectFromUint32(0, &pos);
    FRESetObjectProperty(cursorContext, (const uint8_t*)"pos", pos, 0);
    
    FREObject length;
    FRENewObjectFromUint32(count, &length);
    FRESetObjectProperty(cursorContext, (const uint8_t*)"length", length, 0);
    
    FREObject info;
    FRENewObjectFromUTF8(log->size, (const uint8_t*)log->ptr, &info);
    FRESetObjectProperty(cursorContext, (const uint8_t*)"info", info, 0);
    
    tcxstrdel(log);
    bson_destroy(query);
    bson_destroy(hints);
    bson_destroy(orArray);

    return cursorContext;
}

// (dbContext:Object)
FREObject AirEJDBSync(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    ejdbsyncdb(ejdb);
    
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBDropIndexes(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXDROPALL);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBOptimizeIndexes(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXOP);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBEnsureStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXSTR);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBEnsureIStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXISTR);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBEnsureNumberIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXNUM);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBEnsureArrayIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXARR);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBRebuildStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXSTR | JBIDXREBLD);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBRebuildIStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXISTR | JBIDXREBLD);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBRebuildNumberIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXNUM | JBIDXREBLD);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBRebuildArrayIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXARR | JBIDXREBLD);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBDropStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXSTR | JBIDXDROP);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBDropIStringIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXISTR | JBIDXDROP);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBDropNumberIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXNUM | JBIDXDROP);
    return 0;
}

// (dbContext:Object, collection:String, path:String)
FREObject AirEJDBDropArrayIndex(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    EJDB* ejdb = _GetDatabseFromArguments(ctx, argv);
    if (!ejdb) { _Log("Database is closed"); return 0; }
    
    FREObject collection = argv[1];
    FREObject path = argv[2];
    
    _SetIndex(ejdb, collection, path, JBIDXARR | JBIDXDROP);
    return 0;
}


#pragma mark -
#pragma mark Cursor


FREObject AirEJDBCursorClose(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    ContextData *contextData = 0;
    FREGetContextNativeData(ctx, (void**)&contextData);
    
    FREObject cursorContext = argv[0];
    
    FREObject id;
    uint32_t identifier;
    FREGetObjectProperty(cursorContext, (const uint8_t*)"id", &id, 0);
    FREGetObjectAsUint32(id, &identifier);
    
    CursorData cursorData = contextData->cursors[identifier];
    contextData->cursors.erase(identifier);
    
    ejdbquerydel(cursorData.query); cursorData.query = 0;
    tclistdel(cursorData.results); cursorData.results = 0;
    
    FREObject valid;
    FRENewObjectFromBool(0, &valid);
    FRESetObjectProperty(cursorContext, (const uint8_t*)"valid", valid, 0);
    
    FREObject pos;
    FRENewObjectFromUint32(0, &pos);
    FRESetObjectProperty(cursorContext, (const uint8_t*)"pos", pos, 0);
    
    FREObject length;
    FRENewObjectFromUint32(0, &length);
    FRESetObjectProperty(cursorContext, (const uint8_t*)"length", length, 0);
    
    return 0;
}

FREObject AirEJDBCursorGetField(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    ContextData *contextData = 0;
    FREGetContextNativeData(ctx, (void**)&contextData);
    
    FREObject cursorContext = argv[0];
    FREObject fieldPath = argv[1];
    
    FREObject id;
    uint32_t identifier;
    FREGetObjectProperty(cursorContext, (const uint8_t*)"id", &id, 0);
    FREGetObjectAsUint32(id, &identifier);
    
    FREObject pos;
    uint32_t position;
    FREGetObjectProperty(cursorContext, (const uint8_t*)"pos", &pos, 0);
    FREGetObjectAsUint32(pos, &position);
    
    const char *field;
    uint32_t fieldLength = 0;
    FREGetObjectAsUTF8(fieldPath, &fieldLength, (const uint8_t**)&field);
    
    TCLIST *cursor = contextData->cursors[identifier].results;
    const void *bsdata = TCLISTVALPTR(cursor, position);
    bson_iterator it;
    bson_iterator_from_buffer(&it, (const char*)bsdata);
    bson_type it_type = bson_find_fieldpath_value2(field, fieldLength, &it);
    
    _Log("SEARCHING FOR FIELD: %s", field);
    _Log("FIELD TYPE: %d", it_type);
    
    FREObject object;
    _FRENewObjectFromBSON(&it, it_type, &object);
    
    return object;
}

FREObject AirEJDBCursorGetObject(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
{
    ContextData *contextData = 0;
    FREGetContextNativeData(ctx, (void**)&contextData);
    
    FREObject cursorContext = argv[0];

    FREObject id;
    uint32_t identifier;
    FREGetObjectProperty(cursorContext, (const uint8_t*)"id", &id, 0);
    FREGetObjectAsUint32(id, &identifier);
    
    FREObject pos;
    uint32_t position;
    FREGetObjectProperty(cursorContext, (const uint8_t*)"pos", &pos, 0);
    FREGetObjectAsUint32(pos, &position);
    
    TCLIST *cursor = contextData->cursors[identifier].results;
    const void *bsdata = TCLISTVALPTR(cursor, position);
    bson_iterator it;
    bson_iterator_from_buffer(&it, (const char*)bsdata);
    
    FREObject object;
    _BSONToFREObject(&it, BSON_OBJECT, &object);
    
    return object;
}
