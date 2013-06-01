package com.thejustinwalsh.data
{
	public class EJDBDatabase implements EJDBImplementation
	{
		public function EJDBDatabase(factory:EJDBDatabaseFactoryEnforcer)
		{
			if (!factory) throw new Error("Databases can only be opened with EJDB.open()");
		}

		public function close():void
		{
			
		}
		
		public function get isOpen():Boolean
		{
			return false;
		}
		
		public function ensureCollection(collection:String, options:EJDBCollectionOptions):void
		{
			
		}
		
		public function dropCollection(collection:String, prune:Boolean):void
		{
			
		}

		// ...args (take a single object, an array of objects, or comma seperated object list
		public function save(collection:String, ...args):Array
		{
			return null;
		}

		public function load(collection:String, oid:String):Object
		{
			return null;
		}

		public function remove(collection:String, oid:String):void
		{
			
		}

		public function find(collection:String, query:Object, orQueryArray:Array = null, hints:Object = null):EJDBCursor
		{
			return null;
		}

		public function findOne(collection:String, query:Object, orQueryArray:Array = null, hints:Object = null):Object
		{
			return null;
		}

		public function update(collection:String, query:Object, orQueryArray:Array = null, hints:Object = null):Number
		{
			return Number.NaN;
		}

		public function count(collection:String, query:Object, orQueryArray:Array = null, hints:Object = null):Number
		{
			return Number.NaN;
		}

		public function sync():void
		{
			
		}

		public function dropIndexes(collection:String, path:String):void
		{
			
		}

		public function optimizeIndexes(collection:String, path:String):void
		{
			
		}

		public function ensureStringIndex(collection:String, path:String):void
		{
			
		}

		public function ensureIStringIndex(collection:String, path:String):void
		{
			
		}

		public function ensureNumberIndex(collection:String, path:String):void
		{
			
		}

		public function ensureArrayIndex(collection:String, path:String):void
		{
			
		}

		public function rebuildStringIndex(collection:String, path:String):void
		{
			
		}

		public function rebuildIStringIndex(collection:String, path:String):void
		{
			
		}

		public function rebuildNumberIndex(collection:String, path:String):void
		{
			
		}

		public function rebuildArrayIndex(collection:String, path:String):void
		{
			
		}

		public function dropStringIndex(collection:String, path:String):void
		{
			
		}

		public function dropIStringIndex(collection:String, path:String):void
		{
			
		}

		public function dropNumberIndex(collection:String, path:String):void
		{
			
		}

		public function dropArrayIndex(collection:String, path:String):void
		{
			
		}
	}
}

class EJDBDatabaseFactoryEnforcer {}
