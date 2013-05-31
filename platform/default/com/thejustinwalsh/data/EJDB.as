package com.thejustinwalsh.data
{
	import flash.filesystem.File;

	public class EJDB
	{
		public static function get isSupported():Boolean { return false; }
		
		public static function open(database:File, mode:uint, async:Boolean = false):EJDBImplementation
		{
			throw new Error("Not supported on the default platform");
		}
	}
}
