package com.thejustinwalsh.data
{
	import flash.filesystem.File;

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
			
		public static function get isSupported():Boolean { return false; }
		
		public static function isValidOID(oid:String):Boolean
		{
			return false;
		}
		
		public static function open(database:String, mode:uint = DEFAULT_OPEN_MODE, async:Boolean = false):EJDBImplementation
		{
			throw new Error("Not supported on the default platform");
		}
		
		public static function debugLogging(enabled:Boolean):void
		{
			
		}
	}
}
