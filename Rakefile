task :default => :build

def has_distcc
  if ENV['DISABLE_DISTCC']
    false
  else
    system("which distcc")
  end
end

def has_ccache
  system('which ccache')
end

def compiler_prefix
  if has_ccache
    "ccache "
  elsif has_distcc
    "distcc "
  else
    ""
  end
end

def cmake_cmd(arch)
  marvell_cc = "/usr/local/arm/arm-marvell-linux-gnueabi/bin/arm-linux-gcc"
  marvell_cxx = "/usr/local/arm/arm-marvell-linux-gnueabi/bin/arm-linux-g++"
  arm_cc = "/opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-linux-gcc"
  arm_cxx = "/opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/arm-linux-g++"
  if arch == :arm
    "CC='#{compiler_prefix}#{arm_cc}' CXX='#{compiler_prefix}#{arm_cxx}' cmake "
  elsif arch == :marvell
    "CC='#{compiler_prefix}#{marvell_cc}' CXX='#{compiler_prefix}#{marvell_cxx}' cmake "
  elsif arch == :x86
    "CC='#{compiler_prefix}gcc' CXX='#{compiler_prefix}g++' cmake "
  else
    raise "Invalid machine type: #{arch}"
  end
end

def make_cmd
  if has_distcc
    "make -j18"
  else
    "make -j5"
  end
end

desc "Remove the build directory."
task :clean do
  sh 'rm -rf build'  # Remove compilation targets and cmake output.
  sh 'rm -rf sdk/doxygen'  # Remove Doxygen output.
end

desc "Generate SDK documentation."
task :gen_sdkdoc do
  sh 'cd sdk && doxygen Doxyfile'
end

desc "Release SDK documentation to the public web server."
task :release_sdkdoc => :gen_sdkdoc do
  sh 'rsync -rltpz --delete -e ssh sdk/doxygen/* sdkrelease@dev.onyxcommunity.com:/var/www/dev.onyxcommunity.com/sdk_references/'
end

directory "build/arm"
directory "build/x86"

task :arm_env do
  path = ENV['PATH']
  ENV['PATH'] = "/opt/onyx/arm/bin:/opt/freescale/usr/local/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/bin/:#{path}"
  ENV['ONYX_SDK_ROOT'] = '/opt/onyx/arm'
  ENV['PKG_CONFIG_PATH'] = '/opt/onyx/arm/lib/pkgconfig/'
  ENV['QMAKESPEC'] = '/opt/onyx/arm/mkspecs/qws/linux-arm-g++/'
end

task :x86_env do
  # This is specific to the build server we use.
  path = ENV['PATH']
  ENV['PATH'] = "/opt/qtsdk-2009.04/qt/bin:#{path}"
end

namespace :config do
  namespace :x86 do
    task :static => ["build/x86", :x86_env] do
      sh "cd build/x86 && #{cmake_cmd :x86} -DONYX_BUILD_STATIC_LIB:BOOL=ON ../.."
    end

    task :default => ["build/x86", :x86_env] do
      sh "cd build/x86 && #{cmake_cmd :x86} ../.."
    end
  end

  namespace :arm do
    task :static => [:arm_env, "build/arm"] do
      sh "cd build/arm && #{cmake_cmd :arm} -DONYX_BUILD_STATIC_LIB:BOOL=ON -DBUILD_FOR_ARM:BOOL=ON ../.."
    end
    task :default => [:arm_env, "build/arm"] do
      sh "cd build/arm && #{cmake_cmd :arm} -DBUILD_FOR_ARM:BOOL=ON ../.."
    end
  end
end

namespace :build do
  namespace :x86 do
    desc "Build all packages."
    task :static => "config:x86:static" do
      sh "cd build/x86 && #{make_cmd}"
    end

    desc "Build all packages."
    task :default => "config:x86:default" do
      sh "cd build/x86 && #{make_cmd}"
    end
  end

  namespace :arm do
    desc "Build the ARM version of the SDK."
    task :static => ["config:arm:static", :arm_env] do
      sh "cd build/arm && #{make_cmd}"
    end

    desc "Build the ARM version of all packages."
    task :default => ["config:arm:default", :arm_env] do
      sh "cd build/arm && #{make_cmd}"
    end
  end

  desc "Build the static SDK (x86 and arm)"
  task :static => ["build:x86:static", "build:arm:static"]

  desc "Build the SDK (x86 and arm)"
  task :default => ["build:x86:default", "build:arm:default"]
end


desc "Build all packages (native compiling)."
task :build => "build:static"

desc "Run all tests."
task :test => :build do
  sh "killall -q -9 Xvfb || true"
  sh "cd build/x86 && xvfb-run -a -w 3 make test"
end

libs = [ "libonyx_base.a",
         "libonyx_cms.a",
         "libonyx_data.a",
         "libonyx_screen.a",
         "libonyx_sys.a",
         "libonyx_ui.so",
         "libonyx_wireless.a",
         "libonyx_wpa.a" ]

arm_libs = libs.map {|lib| "build/arm/libs/#{lib}"}
x86_libs = libs.map {|lib| "build/x86/libs/#{lib}"}

arm_libs.each {|lib| file lib => "build:arm:default"}
x86_libs.each {|lib| file lib => "build:x86:default"}

date = `date --rfc-3339=date`.chomp
sdktar = "onyxsdk.tar.gz"
timestamped_sdktar = "onyxsdk-#{date}.tar.gz"

file sdktar do
  dirname = "sdk"
  sh "mkdir -p #{dirname}/lib/x86"
  sh "mkdir -p #{dirname}/lib/arm"
  sh "rm -rf #{dirname}/include"
  arm_libs.each do |lib|
    sh "cp #{lib} #{dirname}/lib/arm/"
  end
  x86_libs.each do |lib|
    sh "cp #{lib} #{dirname}/lib/x86/"
  end
  sh "cp -r code/include #{dirname}/include"
  sh "tar -czf #{sdktar} #{dirname}"
  sh "rm -rf #{dirname}"
end

namespace :tar do
  desc "Create an SDK tar ball ready for release (onyxsdk-<date>.tar.gz)."
  task :sdk => sdktar
end

namespace :release do
  desc "Build and upload the SDK to our web server."
  task :sdk => sdktar do
    sh "scp #{sdktar} sdkrelease@dev.onyxcommunity.com:/var/www/dev.onyxcommunity.com/sdk/#{timestamped_sdktar}"
    sh "ssh sdkrelease@dev.onyxcommunity.com 'cd /var/www/dev.onyxcommunity.com/sdk/ && ln -sf #{timestamped_sdktar} onyxsdk-latest.tar.gz && echo #{timestamped_sdktar} >LATEST.txt'"
  end
end

desc "Run cppcheck."
task :cppcheck do
  sh "cppcheck --error-exitcode=1 --suppressions cppcheck_suppressions.txt -I code/include -I third_party/gtest/include code/src > /dev/null"
end
