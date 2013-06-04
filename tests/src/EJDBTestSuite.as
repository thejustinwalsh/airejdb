package
{
	import asunit.framework.TestSuite;
	
	public class EJDBTestSuite extends TestSuite
	{
		public function EJDBTestSuite()
		{
			super();
			addTest(new EJDBDatabaseTest());
			addTest(new EJDBSaveLoadTest());
			addTest(new EJDBReadmeSampleTest());
			addTest(new EJDBQueryTest());
			addTest(new EJDBUpdateTest());
		}
	}
}