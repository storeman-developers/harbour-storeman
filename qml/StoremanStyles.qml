pragma Singleton
import QtQuick 2.0
import Sailfish.Silica 1.0


QtObject {
    readonly property string commentStyle: "
<style>
  a:link {
    color: " + Theme.primaryColor + ";
  }
  pre {
    color: " + Theme.secondaryColor + ";
    font-family: monospace;
    font-size: " + Theme.fontSizeTiny + "px;
    white-space: pre-wrap;
  }
  blockquote {
    color: " + Theme.secondaryHighlightColor + ";
    font-style: italic;
  }
</style>"
}
