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

#pragma once
#ifndef _air_helpers_h
#define _air_helpers_h

#include <string>

#include "FlashRuntimeExtensions.h"
#include "tcejdb/ejdb.h"
#include "tcejdb/bson.h"

// Debugger
////

void _EnableLog(FREContext ctx, bool enabled);

void _Log(const char *format, ...);

// Databse
////

std::string _GetDatabseIdFromArguments(FREObject argv[]);

std::string _GetDatabseId(FREObject dbContext);

EJDB* _GetDatabseFromArguments(FREContext ctx, FREObject argv[]);

EJDB* _GetDatabse(FREContext ctx, FREObject dbContext);


// Collection
////

void _SetCollectionOptions(FREObject options, EJCOLLOPTS *opts);

EJCOLL* _GetCollection(EJDB *ejdb, FREObject collection, FREObject options);


// Index
////

void _SetIndex(EJDB* ejdb, FREObject collection, FREObject path, int mode);


// Objects
////

bson* _FREObjectToBson(FREObject keys, FREObject values, FREObject types, bson *b = 0, uint32_t depth = 0, bool isQuery = false);

void _SetObjectValue(bson_type type, const char *key, uint32_t knum, FREObject *object, FREObject *value);

void _FRENewObjectFromBSON(bson_iterator *it, bson_type type, FREObject *value);
void _BSONToFREObject(bson_iterator *it, bson_type type, FREObject *object);

#endif
