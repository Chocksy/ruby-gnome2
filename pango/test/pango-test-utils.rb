module PangoTestUtils
 private
 def only_pango_version(major, minor, micro=nil)
   micro ||= 0
   unless Pango::Version.or_later?(major, minor, micro)
     omit("Require Pango >= #{major}.#{minor}.#{micro}")
   end
 end
end
