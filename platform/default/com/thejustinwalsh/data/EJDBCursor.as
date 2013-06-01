package com.thejustinwalsh.data
{
	public class EJDBCursor
	{
		function EJDBCursor()
		{

		}

		public function next():Boolean
		{
			return false;
		}
		
		public function get hasNext():Boolean
		{
			return true;
		}

		public function field(name:String):*
		{
			return null;
		}

		public function object():Object
		{
			return null;
		}

		public function reset():void
		{

		}

		public function get length():uint
		{
			return 0;
		}

		public function get pos():uint
		{
			return 0;
		}

		public function set pos(value:uint):void
		{

		}

		public function close():void
		{
			
		}
		
		public function toArray():Array
		{
			return [];
		}
	}
}