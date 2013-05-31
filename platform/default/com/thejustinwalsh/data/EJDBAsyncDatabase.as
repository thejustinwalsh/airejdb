package com.thejustinwalsh.data
{

	public class EJDBAsyncDatabase implements EJDBImplementation
	{
		public function EJDBAsyncDatabase(dbContext:Object)
		{

		}

		public function close():void
		{

		}
		
		public function get isOpen():Boolean
		{
			return false;
		}
		
		public function ensureCollection(collection:String, options:Object):void
		{
			
		}
		
		public function dropCollection(collection:String, prune:Boolean):void
		{
			
		}

		public function save(collection:String, objects:Array):Array
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

		public function find(collection:String, query:Object, orQueryArray:Array, hints:Object):* // Number, Cursor
		{
			return null;
		}

		public function findOne(collection:String, query:Object, orQueryArray:Array, hints:Object):Object
		{
			return null;
		}

		public function update(collection:String, query:Object, orQueryArray:Array, hints:Object):Number
		{
			return Number.NaN;
		}

		public function count(collection:String, query:Object, orQueryArray:Array, hints:Object):Number
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
