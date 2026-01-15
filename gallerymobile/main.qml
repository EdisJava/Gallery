import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    MyToolbar {
     width: parent.width
     }

    // A nice red rectangle
     Rectangle {
     width: 200; height: 200
     color: "red"
     }

     Rectangle {
      width: 200; height: 200
      color: "green"
      x: 100; y: 100
      Rectangle {
      width: 50; height: 50
      color: "blue"
      x: 100; y: 100
      }
}



}
