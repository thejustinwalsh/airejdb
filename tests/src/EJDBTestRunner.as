package
{
	import flash.system.Capabilities;
	
	import asunit.textui.TestRunner;
	
	public class EJDBTestRunner extends TestRunner
	{
		public function EJDBTestRunner()
		{
			if (Capabilities.isDebugger && Capabilities.os.indexOf("Mac") >= 0) {
				ANE.patch("com.thejustinwalsh.ane.EJDB", begin);
			}
			else {
				begin("com.thejustinwalsh.ejdb-hello", true);
			}
		}
		
		public function begin(extensionId:String, loaded:Boolean):void
		{
			start(EJDBTestSuite);
		}
	}
}