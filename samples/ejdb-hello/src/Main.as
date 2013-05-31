package
{
	import com.thejustinwalsh.data.EJDB;
	import com.thejustinwalsh.data.EJDBCursor;
	import com.thejustinwalsh.data.EJDBDatabase;
	
	import flash.display.Bitmap;
	import flash.display.Sprite;
	import flash.system.Capabilities;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	
	import avmplus.getQualifiedClassName;
	
	public class Main extends Sprite
	{

		private var textField:TextField;
		public function Main()
		{
			if (Capabilities.isDebugger && Capabilities.os.indexOf("Mac") >= 0) {
				ANE.patch("com.thejustinwalsh.ane.EJDB", start);
			}
			else {
				start("com.thejustinwalsh.ejdb-hello", true);
			}
			
		}
		
		public function start(extensionID:String, success:Boolean):void
		{
			log("ANE Status:", extensionID, success);
			log("EJDB Support:", EJDB.isSupported ? "true" : "false");
			try {
				//EJDB.debugLogging(true);
				var db:EJDBDatabase = EJDB.open("hello") as EJDBDatabase;
			}
			catch(e:Error) {
				log(e.message);
				return;
			}
			
			var epicObject:Object = {};
			epicObject.bytes = new ByteArray();
			epicObject.uint = uint.MAX_VALUE;
			epicObject.int = int.MIN_VALUE;
			epicObject.number = Number.PI;
			epicObject.string = "Hello World";
			epicObject.amf = new Bitmap();
			epicObject.object = { nested: "object", forThe: "win" };
			epicObject.array = [0, 1, {something: "tricky"}, 3, new Date()];
			epicObject.date = new Date();
			
			log("In Date: ", (epicObject.date as Date).toDateString(), (epicObject.date as Date).toTimeString(), (epicObject.date as Date).seconds, (epicObject.date as Date).milliseconds);
			
			db.dropCollection("world", true);
			var saved:Array = db.save("world", epicObject, { hello: 'world' }, { testing: [] });
			
			for (var i:int = 0; i < saved.length; ++i) {
				log("Index", i, "=", saved[i]);
			}
			
			var cursor:EJDBCursor = db.find("world3", {});
			while (cursor.next()) {
				var obj:Object = cursor.object();
				log("An object number: ", cursor.field("number"));
			} 
			var array:Array = cursor.toArray();
			cursor.close();
			
			obj = db.findOne("world3", {});
			log("Out Date: ", (obj.date as Date).toDateString(), (obj.date as Date).toTimeString(), (obj.date as Date).seconds, (obj.date as Date).milliseconds);
			log("Look a single object:", obj);
			
			var count:Number = db.count("world3", {});
			log(count, "records exist!");
			
			obj = db.load("world3", saved[1]);
			log("Look a loaded object", obj);
		}
		
		private function log(...args):void
		{
			if (!textField) {
				textField = new TextField();
				textField.multiline = true;
				textField.wordWrap = true;
				textField.width = stage.stageWidth;
				textField.height = stage.stageHeight;
				addChild(textField);
			}
			
			textField.text += args.join(" ") + "\n";
		}
	}
}