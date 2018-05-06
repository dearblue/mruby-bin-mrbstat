#!ruby

MRuby::Build.new do |conf|
  toolchain :gcc

  conf.build_dir = "host"

  enable_debug
  enable_test

  gembox "full-core"

  gem core: "mruby-print"
  gem core: "mruby-bin-mrbc"
  gem core: "mruby-bin-mirb"
  gem core: "mruby-bin-mruby"
  gem File.dirname(__FILE__)
end

MRuby::Build.new("host-opt") do |conf|
  toolchain :gcc

  conf.build_dir = conf.name

  cc.flags << "-O3"

  gem File.dirname(__FILE__)
end

MRuby::Build.new("host-minimal") do |conf|
  toolchain :gcc

  conf.build_dir = conf.name

  cc.defines << "MRB_USE_ETEXT_EDATA"
  cc.flags << "-Os"

  gem File.dirname(__FILE__)
end
