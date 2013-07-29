package com.thejustinwalsh.data
{
	import flash.events.Event;
	import flash.events.StatusEvent;
	import flash.external.ExtensionContext;
	import flash.filesystem.File;
	import flash.desktop.NativeApplication;

	public class EJDB
	{
		public static const JBOREADER:uint = 1 << 0; /**< Open as a reader. */
		public static const JBOWRITER:uint = 1 << 1; /**< Open as a writer. */
		public static const JBOCREAT:uint = 1 << 2;  /**< Create if db file not exists. */
		public static const JBOTRUNC:uint = 1 << 3;  /**< Truncate db on open. */
		public static const JBONOLCK:uint = 1 << 4;  /**< Open without locking. */
		public static const JBOLCKNB:uint = 1 << 5;  /**< Lock without blocking. */
		public static const JBOTSYNC:uint = 1 << 6;  /**< Synchronize every transaction. */
		public static const DEFAULT_OPEN_MODE:uint = (EJDB.JBOWRITER | EJDB.JBOCREAT | EJDB.JBOTSYNC)
		
		public static function get isSupported():Boolean { return true; }
		
		public static function isValidOID(oid:String):Boolean
		{
			if (oid.length !== 24) return false;
			
			for (var i:int = 0; ((oid.charCodeAt(i) >= 0x30 && oid.charCodeAt(i) <= 0x39) || /* 1 - 9 */
				(oid.charCodeAt(i) >= 0x61 && oid.charCodeAt(i) <= 0x66)); /* a - f */
				++i);
			return (i === 24);
		}
		
		public static function open(database:String, mode:uint = DEFAULT_OPEN_MODE, async:Boolean = false):EJDBImplementation
		{
			// Sanatize the database path
			var rootPath:File = File.applicationStorageDirectory.resolvePath(database);
			var fileName:String = database.substr(database.lastIndexOf(File.separator) + 1);
			database = (rootPath.nativePath.lastIndexOf(fileName) == (rootPath.nativePath.length - fileName.length)) ? rootPath.nativePath : rootPath.resolvePath(fileName).nativePath;
			
			// Create the database context
			var dbContext:Object = extContext.call("db.open", database, mode, async);
			if (dbContext.error) throw new Error(dbContext.error + " on path \""+database+"\"");
			
			// Return the proper implementation of the database wrapper
			if (async) return null;
			return EJDBDatabase.construct(dbContext);
		}
		
		public static function debugLogging(enabled:Boolean):void
		{
			extContext.call("debug.logging", enabled);
		}
		
		protected static function onLogEvent(event:StatusEvent):void
		{
			if (event.code != "EJDBLog") return;
			trace("EJDB Log:\t"+event.level);
		}
		
		use namespace ejdb_internal;
		private static var _extContext:ExtensionContext;
		ejdb_internal static function get extContext():ExtensionContext
		{
			if (!_extContext) {
				_extContext = ExtensionContext.createExtensionContext("com.thejustinwalsh.ane.EJDB", "");
				if (!_extContext) throw new Error("Unable to aquire extension context");
				_extContext.addEventListener(StatusEvent.STATUS, onLogEvent, false, 0, true);
				NativeApplication.nativeApplication.addEventListener(Event.EXITING, function(e:Event):void { _extContext.dispose(); _extContext = null; });
			}
			
			return _extContext;
		}
	}
}
