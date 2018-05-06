#!ruby

MRuby::Gem::Specification.new("mruby-bin-mrbstat") do |s|
  s.summary = "print information after mrb_open and exit only"
  s.license = "CC0"
  s.author  = "dearblue"
  s.homepage = "https://github.com/dearblue/mruby-bin-mrbstat"

  if cc.command =~ /\b(g?cc|clang)\d*\b/
    cc.flags << "-Wno-declaration-after-statement"
    cc.flags << "-g3"
  end

  linker.libraries << "execinfo"

  s.bins = %w(mrbstat)
end
