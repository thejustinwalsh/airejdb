# EJDB Native Extension
Adobe Air Native Extension (ANE) for EJDB

## About
[EJDB](https://github.com/Softmotions/ejdb) for iOS, Mac OS X, and Windows on Adobe AIR. EJDB is a fast [MongoDB](http://www.mongodb.org/)-like library for document database storage and querying embedded into your application.

## Usage
```as3
// Open the zoo DB
var db:EJDBDatabase = EJDB.open("zoo", EJDB.DEFAULT_OPEN_MODE | EJDB.JBOTRUNC) as EJDBDatabase;

var parrot1:Object = {
	"name" : "Grenny",
	"type" : "African Grey",
	"male" : true,
	"age" : 1,
	"birthdate" : new Date(),
	"likes" : ["green color", "night", "toys"],
	"extra1" : null
};
var parrot2:Object = {
	"name" : "Bounty",
	"type" : "Cockatoo",
	"male" : false,
	"age" : 15,
	"birthdate" : new Date(),
	"likes" : ["sugar cane"]
};

var oids:Array = db.save("parrots", [parrot1, parrot2]);
trace("Grenny OID: " + parrot1._id);
trace("Bounty OID: " + parrot2._id);

var cursor:EJDBCursor = db.find("parrots",
	{"likes" : "toys"},
	[],
	{"$orderby" : {"name" : 1}}
);

trace("Found " + cursor.length + " parrots");
while (cursor.next()) {
	trace(cursor.field("name") + " likes toys!");
}

cursor.close(); // It IS mandatory to close cursor explicitly to free up resources
db.close(); // Close the database
```

## Considerations
Database files are memory mapped and limited to 2GB on all 32bit platforms. This applies to all supported platforms in the ANE since Adobe only supports 32bit extensions.

On the windows platoform, the native extension is dependent on `tcejdbdll.dll`. You will need to release your AIR application with this dll copied into the working directory of your application, or placed into a shared windows location that is in the windows path enviroment. This makes it less then ideal to debug or run your code from Flash Builder as well. I've found that placing the `tcejdbdll.dll` next to the version of `adt` you are running works quite well for debugging. As far as I know, there is no way to bundle the external DLL into the native extension and have it load from there. If anyone knows a better way to pull it off let me know.

## Building
The native extension is using ruby's rake for it's build system and therefore requires ruby 1.9.3, this is installed by default on OS X Lion.  Additionally, you must have Xcode and the iOS SDK's installed on your system.

To build simply type `rake build` into your terminal from within the root directory of the native extension.  If this is your first time building the build system will generate the file `config/build.yml` and prompt you to edit the file for your system.  The relevant changes are the path to your Adobe Air SDK and what version of the iOS SDK you are using.

After editing your `config/build.yml` file, simply type `rake build` again.  If all goes well you will see the `airejdb.ane` file sitting in your `bin` directory.

Since this is a multiplatform ANE, building is a bit more complicated then usual. If you are running on Mac OS X you will find a file called `ejdb-build.sh` in the `config` directory. Before executing the rake script you will want to run this script via `sh ejdb-build.sh` to generate the Mac and iOS universal binaries of ejdb, as well as download the windows libraries of ejdb.

You will then need to find a way to build the windows MSVC solution found in `platforms/windows/AirEJDB.sln`, I personally use Visual C++ Express 2010 via Parallels running Windows 7. After compiling the MSVC solution, the rake script will compile the iOS and Mac native libraries, then package all available platforms.

## License
airejdb is licensed under a permissive MIT source license. EJDB and Tokyo Cabinet, which this project depends on are licensed under the GNU Lesser General Public License. Fork well my friends.

	Copyright (c) 2013 Justin Walsh, http://thejustinwalsh.com/

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
	OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
