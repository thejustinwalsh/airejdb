package com.thejustinwalsh.data
{
	import flash.utils.ByteArray;
	import flash.utils.getQualifiedClassName;

	internal class EJDBSerializer
	{
		public static function toKeyValuePair(object:Object, keys:Array, values:Array, types:Array):void
		{
			for (var key:String in object) {
				var value:* = object[key];
				describeItem(value, key, keys, values, types);
			}
		}
		
		public static function toTypedArray(array:Array, keys:Array, values:Array, types:Array):void
		{
			for (var i:int = 0, n:int = array.length; i < n; ++i) {
				var value:* = array[i];
				describeItem(value, i.toString(), keys, values, types);
			}
		}
		
		private static function describeItem(value:*, key:String, keys:Array, values:Array, types:Array):void
		{
			var type:String = getQualifiedClassName(value);
			switch (type)
			{
				case  getQualifiedClassName(Boolean):
				case  getQualifiedClassName(int):
				case  getQualifiedClassName(uint):
				case  getQualifiedClassName(Number):
				case  getQualifiedClassName(String):
				case  getQualifiedClassName(ByteArray):
					keys.push(key); values.push(value); types.push(type);
					break;
				
				case getQualifiedClassName(Date):
					var epoch:Number = (value as Date).getTime();
					keys.push(key); values.push(epoch); types.push(type);
					break;
				
				case getQualifiedClassName(Object):
					var o:Object = { keys: [], values: [], types: [] };
					toKeyValuePair(value, o.keys, o.values, o.types);
					keys.push(key); values.push(o); types.push(type);
					break;
				
				case getQualifiedClassName(Array):
					var a:Object = { keys: [], values: [], types: [] };
					toTypedArray(value, a.keys, a.values, a.types);
					keys.push(key); values.push(a); types.push(type);
					break;
					
				default:
					if (value === null) {
						keys.push(key); values.push(value); types.push(getQualifiedClassName(null));
					}
					else {
						var bytes:ByteArray = new ByteArray();
						bytes.writeObject(value);
						bytes.position = 0;
						keys.push(key); values.push(bytes); types.push(getQualifiedClassName(ByteArray));
					}
			}
		}
	}
}