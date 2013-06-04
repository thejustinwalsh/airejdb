package com.thejustinwalsh.data
{
	import flash.utils.getQualifiedClassName;

	public class EJDBDatabase implements EJDBImplementation
	{
		use namespace ejdb_internal;
		private var _dbContext:Object;
		
		ejdb_internal static function construct(dbContext:Object):EJDBDatabase
		{
			var db:EJDBDatabase = new EJDBDatabase(new EJDBDatabaseFactoryEnforcer);
			db._dbContext = dbContext;
			return db;
		}
		
		public function EJDBDatabase(factory:EJDBDatabaseFactoryEnforcer)
		{
			if (!factory) throw new Error("Databases can only be opened with EJDB.open()");
		}

		public function close():void
		{
			EJDB.extContext.call("db.close", _dbContext);
		}
		
		public function get isOpen():Boolean
		{
			return EJDB.extContext.call("db.isOpen", _dbContext) as Boolean;
		}
		
		public function ensureCollection(collection:String, options:EJDBCollectionOptions):void
		{
			EJDB.extContext.call("db.ensureCollection", _dbContext, collection, options);
		}
		
		public function dropCollection(collection:String, prune:Boolean):void
		{
			EJDB.extContext.call("db.dropCollection", _dbContext, collection, prune);
		}

		public function save(collection:String, objects:Array, options:Object = null):Array
		{
			if (!options) options = {};
			var merge:Boolean = options.hasOwnProperty("$merge") && options.$merge;
			
			var keys:Array = [], values:Array = [], types:Array = [];
			for (var i:int = 0, n:int = objects.length; i < n; ++i) {
				var object:Object = objects[i];
				var type:String = getQualifiedClassName(object); 
				if (type == "Object") {
					if (object.hasOwnProperty("_id") && object._id != "" && !EJDB.isValidOID(object._id)) throw new ArgumentError("OID("+object._id+") is invalid");
				
					var objectKeys:Array = [], objectValues:Array = [], objectTypes:Array = [];
					EJDBSerializer.toKeyValuePair(object, objectKeys, objectValues, objectTypes);
					keys.push(objectKeys); values.push(objectValues); types.push(objectTypes);
				}
				else {
					keys.push(null); values.push(null); types.push(null);
				}
			}
			
			var defaultOptions:EJDBCollectionOptions = new EJDBCollectionOptions();
			var oids:Array = EJDB.extContext.call("db.save", _dbContext, collection, keys, values, types, merge, defaultOptions.toObject()) as Array;
			for (i = 0, n = objects.length; i < n; ++i) {
				if (oids[i]) objects[i]._id = oids[i];
			}
			return oids;
		}

		public function load(collection:String, oid:String):Object
		{
			if (!EJDB.isValidOID(oid)) throw new ArgumentError("OID("+oid+") is invalid");
			return EJDB.extContext.call("db.load", _dbContext, collection, oid);
		}

		public function remove(collection:String, oid:String):void
		{
			EJDB.extContext.call("db.remove", _dbContext, collection, oid);
		}

		public function find(collection:String, query:Object, orQueryArray:Array = null, hints:Object = null):EJDBCursor
		{
			if (!orQueryArray) orQueryArray = [];
			if (!hints) hints = {};
			
			var qKeys:Array = [], qValues:Array = [], qTypes:Array = [];
			EJDBSerializer.toKeyValuePair(query, qKeys, qValues, qTypes);
			
			var orKeys:Array = [], orValues:Array = [], orTypes:Array = [];
			EJDBSerializer.toTypedArray(orQueryArray, orKeys, orValues, orTypes);
			
			var hKeys:Array = [], hValues:Array = [], hTypes:Array = [];
			EJDBSerializer.toKeyValuePair(hints, hKeys, hValues, hTypes);
			
			var cursor:Object = EJDB.extContext.call("db.find", _dbContext, collection, 
				qKeys, qValues, qTypes,		// Query
				orKeys, orValues, orTypes,	// OR Array
				hKeys, hValues, hTypes		// Hints
			);
			
			if (cursor == null) cursor = { id: uint.MAX_VALUE, valid: false, pos: 0, length: 0 };
			return new EJDBCursor(cursor);
		}

		public function findOne(collection:String, query:Object, orQueryArray:Array = null, hints:Object = null):Object
		{
			if (!hints) hints = {};
			hints.$max = 1;
			
			var cursor:EJDBCursor = find(collection, query, orQueryArray, hints);
			return (cursor.length > 0) ? cursor.object() : null;
		}

		public function update(collection:String, query:Object, orQueryArray:Array = null, hints:Object = null):Number
		{
			if (!hints) hints = {};
			hints.$onlycount = true;
			
			var cursor:EJDBCursor = find(collection, query, orQueryArray, hints);
			return cursor.length;
		}

		public function count(collection:String, query:Object, orQueryArray:Array = null, hints:Object = null):Number
		{
			if (!hints) hints = {};
			hints.$onlycount = true;
			
			var cursor:EJDBCursor = find(collection, query, orQueryArray, hints);
			return cursor.length;
		}

		public function sync():void
		{
			EJDB.extContext.call("db.sync", _dbContext);
		}

		public function dropIndexes(collection:String, path:String):void
		{
			EJDB.extContext.call("db.dropIndexes", _dbContext, collection, path);
		}

		public function optimizeIndexes(collection:String, path:String):void
		{
			EJDB.extContext.call("db.optimizeIndexes", _dbContext, collection, path);
		}

		public function ensureStringIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.ensureStringIndex", _dbContext, collection, path);
		}

		public function ensureIStringIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.ensureIStringIndex", _dbContext, collection, path);
		}

		public function ensureNumberIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.ensureNumberIndex", _dbContext, collection, path);
		}

		public function ensureArrayIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.ensureArrayIndex", _dbContext, collection, path);
		}

		public function rebuildStringIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.rebuildStringIndex", _dbContext, collection, path);
		}

		public function rebuildIStringIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.rebuildIStringIndex", _dbContext, collection, path);
		}

		public function rebuildNumberIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.rebuildNumberIndex", _dbContext, collection, path);
		}

		public function rebuildArrayIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.rebuildArrayIndex", _dbContext, collection, path);
		}

		public function dropStringIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.dropStringIndex", _dbContext, collection, path);
		}

		public function dropIStringIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.dropIStringIndex", _dbContext, collection, path);
		}

		public function dropNumberIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.dropNumberIndex", _dbContext, collection, path);
		}

		public function dropArrayIndex(collection:String, path:String):void
		{
			EJDB.extContext.call("db.dropArrayIndex", _dbContext, collection, path);
		}
	}
}

class EJDBDatabaseFactoryEnforcer {}
