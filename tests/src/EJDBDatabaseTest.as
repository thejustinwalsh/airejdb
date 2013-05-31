package
{
	import com.thejustinwalsh.data.EJDB;
	import com.thejustinwalsh.data.EJDBCollectionOptions;
	import com.thejustinwalsh.data.EJDBDatabase;
	
	import flash.filesystem.File;
	
	import asunit.framework.TestCase;
	
	public class EJDBDatabaseTest extends TestCase
	{
		public function EJDBDatabaseTest(testMethod:String=null)
		{
			super(testMethod);
		}
		
		protected override function setUp():void
		{
			
		}
		
		
		protected override function tearDown():void
		{
			File.applicationStorageDirectory.resolvePath("ejdb-database-test-1").deleteFile();
			File.applicationStorageDirectory.resolvePath("ejdb-database-test-2").deleteFile();
		}
		
		public function testOpenClose():void
		{
			assertNotNull(EJDB.JBOWRITER);
			assertNotNull(EJDB.JBOCREAT);
			assertNotNull(EJDB.JBOTRUNC);
			
			var db:EJDBDatabase = EJDB.open("ejdb-database-test-1", EJDB.JBOWRITER | EJDB.JBOCREAT | EJDB.JBOTRUNC) as EJDBDatabase;
			assertNotNull(db);
			assertTrue(db.isOpen);
			
			db.close();
			assertFalse(db.isOpen);
			
			var databaseFile:File = File.applicationStorageDirectory.resolvePath("ejdb-database-test-1");
			assertTrue(databaseFile.exists);
		}
		
		public function testEnsureAndRemoveCollection():void
		{
			var db:EJDBDatabase = EJDB.open("ejdb-database-test-2", EJDB.JBOWRITER | EJDB.JBOCREAT | EJDB.JBOTRUNC) as EJDBDatabase;
			assertTrue(db.isOpen);
			
			var options:EJDBCollectionOptions = new EJDBCollectionOptions();
			options.cachedrecords = 10000;
			options.compressed = true;
			options.large = true;
			options.records = 1000000;
			
			db.ensureCollection("c1", options);
			var collectionFile:File = File.applicationStorageDirectory.resolvePath("ejdb-database-test-2_c1");
			assertTrue(collectionFile.exists);
			
			db.dropCollection("c1", true);
			collectionFile = File.applicationStorageDirectory.resolvePath("ejdb-database-test-2_c1");
			assertFalse(collectionFile.exists);
			
			// TODO: Error handeling?
			db.close();
			db.dropCollection("c1", true);
		}
	}
}