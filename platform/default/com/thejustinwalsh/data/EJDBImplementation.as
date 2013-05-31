package com.thejustinwalsh.data
{
	internal interface EJDBImplementation
	{
		function close():void;
		function get isOpen():Boolean;

		function ensureCollection(collection:String, options:Object):void;
		function dropCollection(collection:String, prune:Boolean):void;

		function save(collection:String, objects:Array):Array;
		function load(collection:String, oid:String):Object;
		function remove(collection:String, oid:String):void;
		
		function find(collection:String, query:Object, orQueryArray:Array, hints:Object):*
		function findOne(collection:String, query:Object, orQueryArray:Array, hints:Object):Object
		function update(collection:String, query:Object, orQueryArray:Array, hints:Object):Number	
		function count(collection:String, query:Object, orQueryArray:Array, hints:Object):Number

		function sync():void;

		function dropIndexes(collection:String, path:String):void;
		function optimizeIndexes(collection:String, path:String):void;

		function ensureStringIndex(collection:String, path:String):void;
		function ensureIStringIndex(collection:String, path:String):void;
		function ensureNumberIndex(collection:String, path:String):void;
		function ensureArrayIndex(collection:String, path:String):void;

		function rebuildStringIndex(collection:String, path:String):void;
		function rebuildIStringIndex(collection:String, path:String):void;
		function rebuildNumberIndex(collection:String, path:String):void;
		function rebuildArrayIndex(collection:String, path:String):void;

		function dropStringIndex(collection:String, path:String):void;
		function dropIStringIndex(collection:String, path:String):void;
		function dropNumberIndex(collection:String, path:String):void;
		function dropArrayIndex(collection:String, path:String):void;
	}
}
