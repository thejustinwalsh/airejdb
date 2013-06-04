package
{
	import com.thejustinwalsh.data.EJDB;
	import com.thejustinwalsh.data.EJDBCursor;
	import com.thejustinwalsh.data.EJDBDatabase;
	
	import flash.filesystem.File;
	import flash.utils.getQualifiedClassName;
	
	import asunit.framework.TestCase;
	
	public class EJDBQueryTest extends TestCase
	{
		private var now:Date;
		private var db:EJDBDatabase;
		
		public function EJDBQueryTest(testMethod:String=null)
		{
			super(testMethod);
		}
		
		protected override function setUp():void
		{
			now = new Date();
			db = EJDB.open("ejdb-query-test", EJDB.JBOWRITER | EJDB.JBOCREAT | EJDB.JBOTRUNC) as EJDBDatabase;
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
			
			File.applicationStorageDirectory.resolvePath("ejdb-query-test").deleteFile();
			File.applicationStorageDirectory.resolvePath("ejdb-query-test_parrots").deleteFile();
		}
		
		public function testQuery1():void
		{
			// Find
			var cursor:EJDBCursor = db.find("parrots", {});
			assertNotNull(cursor);
			assertEquals(2, cursor.length);
			
			var c:int = 0;
			while (cursor.next()) {
				++c;
				var rv:Object = cursor.object();
				assertNotNull(rv);
				
				assertEquals(getQualifiedClassName(String), getQualifiedClassName(rv["_id"]));
				assertEquals(getQualifiedClassName(String), getQualifiedClassName(rv["name"]));
				assertEquals(getQualifiedClassName(int), getQualifiedClassName(rv["age"]));
				assertEquals(getQualifiedClassName(Date), getQualifiedClassName(rv["birthdate"]));
				assertEquals(getQualifiedClassName(Boolean), getQualifiedClassName(rv["male"]));
				assertEquals(getQualifiedClassName(null), getQualifiedClassName(rv["extra1"]));
				assertEquals(getQualifiedClassName(Array), getQualifiedClassName(rv["likes"]));
				assertTrue(rv["likes"].length > 0);
			}
			assertEquals(2, c);
			
			// Count
			var count:Number = db.count("parrots", {});
			assertEquals(2, count);
			
			// findOne
			var obj:Object = db.findOne("parrots", {});
			assertNotNull(obj);
			assertEquals(getQualifiedClassName(String), getQualifiedClassName(obj["name"]));
			assertEquals(getQualifiedClassName(int), getQualifiedClassName(obj["age"]));
			assertEquals(getQualifiedClassName(RegExp), getQualifiedClassName(obj["regex"]));
		}
		
		public function testQuery2():void
		{
			var cursor:EJDBCursor = db.find("parrots", {name : /(grenny|bounty)/ig}, [], {$orderby : {name : 1}});
			assertNotNull(cursor);
			assertEquals(2, cursor.length);
			
			for (var c:int = 0; cursor.next(); ++c) {
				var rv:Object = cursor.object();
				if (c != 0) continue;
				
				assertEquals("Bounty", rv["name"]);
				assertEquals("Bounty", cursor.field("name"));
				assertEquals("Cockatoo", rv["type"]);
				assertEquals("Cockatoo", cursor.field("type"));
				assertEquals(false, rv["male"]);
				assertEquals(false, cursor.field("male"));
				assertEquals(15, rv["age"]);
				assertEquals(15, cursor.field("age"));
				assertEquals(now.toString(), rv["birthdate"].toString());
				assertEquals(now.toString(), cursor.field("birthdate").toString());
				assertEquals("sugar cane", rv["likes"].join(","));
				assertEquals("sugar cane", cursor.field("likes").join(","));
			}
		}
		
		public function testQuery3():void
		{
			var cursor:EJDBCursor = db.find("parrots", {}, [{name : "Grenny"}, {name : "Bounty"}], {$orderby : {name : 1}});
			assertNotNull(cursor);
			assertEquals(2, cursor.length);
			
			for (var c:int = 0; cursor.next(); ++c) {
				var rv:Object = cursor.object();
				if (c != 1) continue;
				assertEquals("Grenny", rv["name"]);
				assertEquals("Grenny", cursor.field("name"));
				assertEquals("African Grey", rv["type"]);
				assertEquals("African Grey", cursor.field("type"));
				assertEquals(true, rv["male"]);
				assertEquals(true, cursor.field("male"));
				assertEquals(1, rv["age"]);
				assertEquals(1, cursor.field("age"));
				assertEquals(now.toString(), rv["birthdate"].toString());
				assertEquals(now.toString(), cursor.field("birthdate").toString());
				assertEquals("green color,night,toys", rv["likes"].join(","));
				assertEquals("green color,night,toys", cursor.field("likes").join(","));
			}
			
			cursor.reset();
			for (c = 0; cursor.next(); ++c);
			assertEquals(2, c);
			
			cursor.close();
		}
	}
}