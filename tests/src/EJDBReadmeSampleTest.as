package
{
	import com.thejustinwalsh.data.EJDB;
	import com.thejustinwalsh.data.EJDBCursor;
	import com.thejustinwalsh.data.EJDBDatabase;
	
	import flash.filesystem.File;
	
	import asunit.framework.TestCase;
	
	public class EJDBReadmeSampleTest extends TestCase
	{
		public function EJDBReadmeSampleTest(testMethod:String=null)
		{
			super(testMethod);
		}
		
		protected override function setUp():void
		{

		}
		
		
		protected override function tearDown():void
		{
			File.applicationStorageDirectory.resolvePath("zoo").deleteFile();
			File.applicationStorageDirectory.resolvePath("zoo_parrots").deleteFile();
		}
		
		public function testSample():void
		{
			// Open the zoo DB
			var db:EJDBDatabase = EJDB.open("zoo", EJDB.DEFAULT_OPEN_MODE | EJDB.JBOTRUNC) as EJDBDatabase;
			assertNotNull(db);
			
			var parrot1:Object = {
				"name" : "Grenny",
				"type" : "African Grey",
				"male" : true,
				"age" : 1,
				"birthdate" : new Date(),
				"likes" : ["green color", "night", "toys"],
				"extra1" : null
			};
			var parrot2:Object = {
				"name" : "Bounty",
				"type" : "Cockatoo",
				"male" : false,
				"age" : 15,
				"birthdate" : new Date(),
				"likes" : ["sugar cane"]
			};
			
			var oids:Array = db.save("parrots", [parrot1, parrot2]);
			trace("Grenny OID: " + parrot1._id);
			trace("Bounty OID: " + parrot2._id);
			
			var cursor:EJDBCursor = db.find("parrots",
				{"likes" : "toys"},
				[],
				{"$orderby" : {"name" : 1}}
			);
			
			trace("Found " + cursor.length + " parrots");
			assertEquals(1, cursor.length);
			
			while (cursor.next()) {
				trace(cursor.field("name") + " likes toys!");
				assertEquals("Grenny", cursor.field("name"));
			}
			
			cursor.close(); // It IS mandatory to close cursor explicitly to free up resources
			db.close(); // Close the database
		}
	}
}