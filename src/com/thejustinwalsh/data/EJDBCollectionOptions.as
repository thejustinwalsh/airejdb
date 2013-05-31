package com.thejustinwalsh.data
{
	public class EJDBCollectionOptions extends Object
	{
		// cachedrecords : Max number of cached records in shared memory segment. Default: 0
		// records : Estimated number of records in this collection. Default: 65535.
		// large : Specifies that the size of the database can be larger than 2GB. Default: false
		// compressed : If true collection records will be compressed with DEFLATE compression. Default: false.
		
		public var cachedrecords:uint = 0;
		public var records:uint = 65535;
		public var large:Boolean = false;
		public var compressed:Boolean = false;
		
		public function toObject():Object {
			return { 
				cachedrecords: cachedrecords,
				records: records,
				large: large,
				compressed: compressed
			};
		}
	}
}