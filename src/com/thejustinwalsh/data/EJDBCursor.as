package com.thejustinwalsh.data
{
	public class EJDBCursor
	{
		use namespace ejdb_internal;
		private var _cursorContext:Object;
		
		function EJDBCursor(context:Object)
		{
			_cursorContext = context;
			_cursorContext.noNext = true;
		}

		public function next():Boolean
		{
			if (_cursorContext.noNext) {
				_cursorContext.noNext = false;
				return _cursorContext.length > 0;
			}
			else if (_cursorContext.pos + 1 < _cursorContext.length) {
				_cursorContext.pos++;
				return true;
			}
			
			return false;
		}
		
		public function get hasNext():Boolean
		{
			if (!_cursorContext.valid) throw new Error("Cursor is closed");
			return (_cursorContext.noNext && _cursorContext.length > 0) || (_cursorContext.pos + 1 < _cursorContext.length);
		}

		public function field(name:String):*
		{
			if (!_cursorContext.valid) throw new Error("Cursor is closed");
			return EJDB.extContext.call("cursor.field", _cursorContext, name);
		}

		public function object():Object
		{
			if (!_cursorContext.valid) throw new Error("Cursor is closed");
			return EJDB.extContext.call("cursor.object", _cursorContext);
		}

		public function reset():void
		{
			_cursorContext.pos = 0;
			_cursorContext.noNext = true;
		}

		public function get length():uint
		{
			return _cursorContext.length;
		}

		public function get pos():uint
		{
			return _cursorContext.pos;
		}

		public function set pos(value:uint):void
		{
			if (value < 0) {
				value = _cursorContext.length + value;
			}
			if (value >= 0 && _cursorContext.length > 0) {
				value = (value >= _cursorContext.length) ? _cursorContext.length - 1 : value;
			} else {
				value = 0;
			}
			
			_cursorContext.pos = value;
			_cursorContext.noNext = false;
		}

		public function close():void
		{
			if (!_cursorContext.valid) throw new Error("Cursor is not open");
			EJDB.extContext.call("cursor.close", _cursorContext);
		}
		
		public function log():String
		{
			return _cursorContext.info;
		}
		
		public function toArray():Array
		{
			if (!_cursorContext.valid) throw new Error("Cursor is closed");
			var curPos:uint = pos;
			var objects:Array = [];
			
			reset();
			while (next()) {
				objects.push(object());
			} 
			
			pos = curPos;
			return objects;
		}
	}
}