import QtQuick 2.0

ListModel {

    ListElement {
        userId: 1
        created: qsTrId("orn-just-now")
        commentId: 2
        //: https://simple.wikipedia.org/wiki/42_(answer)
        //% "Deep Thought"
        userName: qsTrId("orn-dcm-user2")
        text: "42"
        parentId: 1
        parentUserName: qsTrId("orn-dcm-user1")
    }

    ListElement {
        userId: 0
        //% "7.5 million years ago"
        created: qsTrId("orn-hint-commentdelegate-created")
        commentId: 1
        //: https://simple.wikipedia.org/wiki/42_(answer)
        //% "A little white mice"
        userName: qsTrId("orn-dcm-user1")
        //: https://simple.wikipedia.org/wiki/42_(answer)
        //% "What is the Answer to the Ultimate Question of Life, the Universe, and Everything?"
        text: qsTrId("orn-dcm-question")
    }
}
