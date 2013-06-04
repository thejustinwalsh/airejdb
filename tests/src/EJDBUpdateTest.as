package
{
	import com.thejustinwalsh.data.EJDB;
	import com.thejustinwalsh.data.EJDBDatabase;
	
	import flash.filesystem.File;
	
	import asunit.framework.TestCase;
	
	public class EJDBUpdateTest extends TestCase
	{
		private var now:Date;
		private var db:EJDBDatabase;
		
		public function EJDBUpdateTest(testMethod:String=null)
		{
			super(testMethod);
		}
		
		protected override function setUp():void
		{
			now = new Date();
			db = EJDB.open("ejdb-update-test", EJDB.JBOWRITER | EJDB.JBOCREAT | EJDB.JBOTRUNC) as EJDBDatabase;
			assertNotNull(db);
			assertNotNull(now)
			
			var parrot1:Object = {
				"name" : "Grenny",
				"type" : "African Grey",
				"male" : true,
				"age" : 1,
				"birthdate" : now,
				"likes" : ["green color", "night", "toys"],
				"extra1" : null,
				"regex" : /hello.*/xgim
			};
			var parrot2:Object = {
				"name" : "Bounty",
				"type" : "Cockatoo",
				"male" : false,
				"age" : 15,
				"birthdate" : now,
				"likes" : ["sugar cane"],
				"extra1" : null
			};
			db.save("parrots", [parrot1, parrot2]);
		}
		
		protected override function tearDown():void
		{
			assertNotNull(db);
			db.close();
			
			File.applicationStorageDirectory.resolvePath("ejdb-update-test").deleteFile();
			File.applicationStorageDirectory.resolvePath("ejdb-update-test_parrots").deleteFile();
		}
		
		public function testUpdate1():void
		{
			// TODO: Only log when explain is true... How to handle log whith synchronous callbacks?
			var count:Number = db.update("parrots", {"name" : {"$icase" : "GRENNY"}, "$inc" : {"age" : 10}}, null, {"$explain" : true});
			assertEquals(1, count);
			
			var parrot:Object = db.findOne("parrots", {"age" : 11});
			assertNotNull(parrot);
			assertEquals("Grenny", parrot["name"]);
			
			var oids:Array = db.save("parrots", [{"_id" :parrot["_id"], "extra1" : 1}], {"$merge" : true});
			assertNotNull(oids);
			assertEquals(1, oids.length);
			
			parrot = db.load("parrots", oids[0]);
			assertNotNull(parrot);
			assertEquals("Grenny", parrot["name"]);
			assertEquals(1, parrot["extra1"]);
			
			count = db.update("parrots", {"_id" : {"$in" : oids}, "$set" : {"stime" : (new Date)}});
			assertEquals(1, count);
		}
		
		public function testCreateCollectionOn$upsert():void
		{
			var count:Number = db.update("upsertcoll", {foo : "bar", $upsert : {foo : "bar"}});
			assertEquals(1, count);
			
			var object:Object = db.findOne("upsertcoll", {foo : "bar"});
			assertNotNull(object);
			assertEquals("bar", object.foo);
			
			db.dropCollection("upsertcoll", true);
		}
	}
}