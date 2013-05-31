package
{
	import asunit.framework.TestSuite;
	
	public class EJDBTestSuite extends TestSuite
	{
		public function EJDBTestSuite()
		{
			super();
			addTest(new EJDBDatabaseTest("testOpenClose"));
			addTest(new EJDBDatabaseTest("testEnsureAndRemoveCollection"));
			addTest(new EJDBSaveLoadTest("testSaveLoad"));
			addTest(new EJDBReadmeSampleTest("testSample"));
		}
	}
}