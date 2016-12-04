module.exports = function(minified) {
  var clayConfig = this;

  /////////////////////////////////////////////////////////////////////////////
  // Clean up input before sending to watch.
  /////////////////////////////////////////////////////////////////////////////
  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
//    clayConfig.getItemByMessageKey('CUSTOM_STR').on('change', function() {
//      this.set(this.get().trim());
//    });
  });

};
