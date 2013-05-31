# Copyright (c) 2013 Justin Walsh, http://thejustinwalsh.com/
# 
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

require "yaml"
require "Shellwords"

PROJECT = "airejdb"
ROOT = File.dirname(__FILE__)

$build = []
$config = "debug"

task :default, [:config] => [:build] do |t, args|
end

desc "Bootstrap the environment by copying our template _build.yml to build.yml and prompting the user to edit the file"
task :bootstrap, [:config] do |t, args|
	unless File.exists?('config/build.yml')
		FileUtils.copy('config/_build.yml', 'config/build.yml')
		abort("## Edit config/build.yml to match your environment before continuing!")
	end
end

task :load, [:config] => [:bootstrap] do |t, args|
	# Default our build configuration to debug
	$config = args[:config] || $config;

	# Load our build configuration from our yaml file
	yaml = YAML::load(File.open('config/build.yml'))
	$build = yaml[$config]
end

desc "Build the native extension"
task :build, [:config] => [:load, :swc, :default_swc, :macos, :ios, :android, :windows] do |t, args|
	airsdk = "#{Shellwords.escape($build['airsdk'])}"
	adt = "#{airsdk}/bin/adt"
	ane = "#{ROOT}/bin/airejdb.ane"
	xml = "#{ROOT}/src/extension.xml"
	swc = "#{ROOT}/bin/#{PROJECT}.swc"
	swf_dir = "#{ROOT}/bin/#{PROJECT}"

	platforms = {
		"default" => {
			"target" 	=> "default",
			"libdir" 	=> "#{ROOT}/bin/default/#{PROJECT}",
			"lib"	 	=> "library.swf"
		},
		"macos" => {
			"target" 	=> "MacOS-x86",
			"libdir" 	=> "#{ROOT}/platform/macos/bin",
			"lib"	 	=> "."
		},
		"ios" => {
			"target" 	=> "iPhone-ARM",
			"options"	=> "#{ROOT}/src/platformoptions.xml",
			"libdir" 	=> "#{ROOT}/platform/ios/bin/",
			"lib"	 	=> "libairejdb.a"
		},
		#"ios-simulator" => {
		#	"target" 	=> "iPhone-x86",
		#	"options"	=> "#{ROOT}/src/platformoptions.xml",
		#	"libdir" 	=> "#{ROOT}/platform/ios/bin/",
		#	"lib"	 	=> "libairejdb.a"
		#},
		"windows" => {
			"target"	=> "Windows-x86",
			"libdir"	=> "#{ROOT}/platform/windows/bin",
			"lib"	 	=> "."
		}
	}

	adt_args = "-package -target ane #{ane} #{xml} -swc #{swc} "
	platforms.each_key {|key|
		target = platforms[key]["target"]
		lib_dir = platforms[key]["libdir"]
		lib = platforms[key]["lib"]
		options = platforms[key]["options"]

		adt_args += "-platform #{target} "
		adt_args += "-C #{swf_dir} library.swf " if target != "default"
		adt_args += "-C #{lib_dir} #{lib} "
		adt_args += "-platformoptions #{options} " if options
	}

	# Run adt and package our ane
	sh "#{adt} #{adt_args}" do |ok, res|
		fail "## adt failed with exitstatus #{res.exitstatus}" if !ok
	end	
end

desc "Build the swc and library.swf files needed for ane packaging"
task :swc, [:config] => [:load] do |t, args|
	debug = ($config == "debug" ? "true" : "false")
	debug_define = "CONFIG::debug," + ($config == "debug" ? "true" : "false")
	airsdk = "#{Shellwords.escape($build['airsdk'])}"
	compc = "#{airsdk}/bin/compc"
	flexlib = "#{airsdk}/frameworks"
	output_dir = "#{ROOT}/bin"
	library = "#{output_dir}/#{PROJECT}.swc"
	library_dir = "#{output_dir}/#{PROJECT}"
	src = "#{ROOT}/src"

	# Remove the swc output directory, then build the swc into a directory
	rm_rf library_dir if File.directory? library_dir
	sh "#{compc} -directory=true -output=#{library_dir} -debug=#{debug} -define=#{debug_define} +flexlib #{flexlib} +configname=air -source-path+=#{src} -include-sources #{src}" do |ok, res|
		fail "## compc failed with exitstatus #{res.exitstatus}" if !ok
	end

	# Build the swc
	sh "#{compc} -output=#{library} -debug=#{debug} -define=#{debug_define} +flexlib #{flexlib} +configname=air -source-path+=#{src} -include-sources #{src}" do |ok, res|
		fail "## compc failed with exitstatus #{res.exitstatus}" if !ok
	end
end

desc "Build the default library for AS only implementations"
task :default_swc, [:config] => [:load] do |t, args|
	debug = ($config == "debug" ? "true" : "false")
	debug_define = "CONFIG::debug," + ($config == "debug" ? "true" : "false")
	airsdk = "#{Shellwords.escape($build['airsdk'])}"
	compc = "#{airsdk}/bin/compc"
	flexlib = "#{airsdk}/frameworks"
	output_dir = "#{ROOT}/bin/default"
	library = "#{output_dir}/#{PROJECT}.swc"
	library_dir = "#{output_dir}/#{PROJECT}"
	src = "#{ROOT}/platform/default"

	# Remove the swc output directory, then build the swc into a directory
	mkdir output_dir unless File.directory? output_dir
	rm_rf library_dir if File.directory? library_dir
	sh "#{compc} -directory=true -output=#{library_dir} -debug=#{debug} -define=#{debug_define} +flexlib #{flexlib} +configname=air -source-path+=#{src} -include-sources #{src}" do |ok, res|
		fail "## compc failed with exitstatus #{res.exitstatus}" if !ok
	end

	# Build the swc
	sh "#{compc} -output=#{library} -debug=#{debug} -define=#{debug_define} +flexlib #{flexlib} +configname=air -source-path+=#{src} -include-sources #{src}" do |ok, res|
		fail "## compc failed with exitstatus #{res.exitstatus}" if !ok
	end
end

desc "Build the native library for Mac OS"
task :macos, [:config] => [:load] do |t, args|
	next unless which "xcodebuild"
	project = "#{ROOT}/platform/macos/AirEJDB.xcodeproj"
	target = 'AirEJDB'
	airsdk = Shellwords.escape($build['airsdk'])
	configuration = $config.capitalize
	symroot = "#{ROOT}/platform/macos/build"

	# Inform Xcode of the AIR_SDK location so that the build can locate the air framework and header, then build the native library
	sh "export AIR_SDK=\"#{airsdk}\"; xcodebuild -project #{project} -configuration #{configuration} -target #{target} build SYMROOT=#{symroot}" do |ok, res|
		fail "## xcodebuild failed with exitstatus #{res.exitstatus}" if !ok
	end
end

desc "Build the native library for iOS"
task :ios, [:config] => [:load] do |t, args|
	next unless which "xcodebuild"
	project = "#{ROOT}/platform/ios/AirEJDB.xcodeproj"
	target = 'AirEJDB'
	airsdk = Shellwords.escape($build['airsdk'])
	iossdk = $build['iossdk']
	configuration = $config.capitalize
	symroot = "#{ROOT}/platform/ios/build"

	# Inform Xcode of the AIR_SDK location so that the build can locate the air framework header, then build the native library
	sh "export AIR_SDK=\"#{airsdk}\"; xcodebuild -project #{project} -sdk #{iossdk} -configuration #{configuration} -target #{target} build SYMROOT=#{symroot}" do |ok, res|
		fail "## xcodebuild failed with exitstatus #{res.exitstatus}" if !ok
	end
end

desc "Build the native library for Android"
task :android, [:config] => [:load] do |t, args|
	next #unless which "xcodebuild"
	project = "#{ROOT}/platform/android/"
	target = ''
	airsdk = Shellwords.escape($build['airsdk'])
	configuration = $config.capitalize
	symroot = "#{ROOT}/platform/macos/build"

	# Inform Xcode of the AIR_SDK location so that the build can locate the air framework and header, then build the native library
	#sh "export AIR_SDK=\"#{airsdk}\"; xcodebuild -project #{project} -configuration #{configuration} -target #{target} build SYMROOT=#{symroot}" do |ok, res|
	#	fail "## xcodebuild failed with exitstatus #{res.exitstatus}" if !ok
	#end
end

desc "Build the native library for Windows"
task :windows, [:config] => [:load] do |t, args|
	next #unless which "xcodebuild"
	project = "#{ROOT}/platform/windows/"
	target = ''
	airsdk = Shellwords.escape($build['airsdk'])
	configuration = $config.capitalize
	symroot = "#{ROOT}/platform/macos/build"

	# Inform Xcode of the AIR_SDK location so that the build can locate the air framework and header, then build the native library
	#sh "export AIR_SDK=\"#{airsdk}\"; xcodebuild -project #{project} -configuration #{configuration} -target #{target} build SYMROOT=#{symroot}" do |ok, res|
	#	fail "## xcodebuild failed with exitstatus #{res.exitstatus}" if !ok
	#end
end

# http://stackoverflow.com/questions/2108727/which-in-ruby-checking-if-program-exists-in-path-from-ruby
def which(cmd)
  exts = ENV['PATHEXT'] ? ENV['PATHEXT'].split(';') : ['']
  ENV['PATH'].split(File::PATH_SEPARATOR).each do |path|
    exts.each { |ext|
      exe = File.join(path, "#{cmd}#{ext}")
      return exe if File.executable? exe
    }
  end
  return nil
end