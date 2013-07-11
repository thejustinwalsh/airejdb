package
{
	import com.thejustinwalsh.data.EJDB;
	import com.thejustinwalsh.data.EJDBDatabase;
	
	import flash.filesystem.File;
	import flash.utils.ByteArray;
	
	import asunit.framework.TestCase;
	
	import avmplus.getQualifiedClassName;
	
	public class EJDBSaveLoadTest extends TestCase
	{
		private var now:Date;
		private var db:EJDBDatabase;
		
		public function EJDBSaveLoadTest(testMethod:String=null)
		{
			super(testMethod);
		}
		
		protected override function setUp():void
		{
			now = new Date();
			db = EJDB.open("ejdb-save-load-test", EJDB.JBOWRITER | EJDB.JBOCREAT | EJDB.JBOTRUNC) as EJDBDatabase;
			assertNotNull(db);
			assertNotNull(now);
		}
		
		
		protected override function tearDown():void
		{
			assertNotNull(db);
			db.close();
			
			File.applicationStorageDirectory.resolvePath("ejdb-save-load-test").deleteFile();
		}
		
		public function testSaveLoad():void
		{
			assertNotNull(db);
			assertTrue(db.isOpen);
			
			var parrot1:Object = {
				"name" : "Grenny",
				"type" : "African Grey",
				"male" : true,
				"age" : 1,
				"birthdate" : now,
				"likes" : ["green color", "night", "toys"],
				"extra1" : null
			};
			var parrot2:Object = {
				"name" : "Bounty",
				"type" : "Cockatoo",
				"male" : false,
				"age" : 15,
				"birthdate" : now,
				"percentFun": 0.33,
				"likes" : ["sugar cane"],
				"extra1" : null
			};
			
			var oids:Array = db.save("parrots", [parrot1, null, parrot2]);
			assertNotNull(oids);
			assertEquals(3, oids.length);
			
			assertEquals(parrot1["_id"], oids[0]);
			assertEquals(null, oids[1]);
			assertEquals(parrot2["_id"], oids[2]);
			
			var parrot2Clone:Object = db.load("parrots", parrot2["_id"]);
			assertNotNull(parrot2Clone);
			assertEquals(parrot2Clone._id, parrot2["_id"]);
			assertEquals(parrot2Clone.name, "Bounty");
			assertEquals(parrot2Clone.percentFun, 0.33);
			
			db.dropCollection("parrots", true);
		}
		
		public function testSaveLoadBuffer():void
		{
			assertNotNull(db);
			assertTrue(db.isOpen);
			
			var secret:ByteArray = new ByteArray();
			secret.writeUTFBytes("Some binary secret");
			
			var sally:Object = {
				name: "Sally",
				mood: "Angry",
				secret: secret
			};
			
			var molly:Object = {
				name: "Molly",
				mood: "Very angry",
				secret: null
			};
			
			var oids:Array = db.save("birds", [sally]);
			assertNotNull(oids);
			assertEquals(1, oids.length);
			assertNotNull(sally["_id"]);
			
			var sallyOid:String = sally._id;
			var sallyObj:Object = db.load("birds", sallyOid);
			assertEquals(getQualifiedClassName(ByteArray), getQualifiedClassName(sallyObj["secret"]));
			assertEquals("Some binary secret", (sallyObj.secret as ByteArray).readUTFBytes((sallyObj.secret as ByteArray).bytesAvailable));			
			
			oids = db.save("birds", [sally, molly]);
			assertNotNull(oids);
			assertTrue(oids.indexOf(sallyOid) !== -1);
			
			db.dropCollection("birds", true);
		}
	}
}