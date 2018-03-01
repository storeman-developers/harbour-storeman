import QtQuick 2.0

ListModel {

    function findItem(id) {
        return id === 0 ? get(1).commentData : null
    }

    Component.onCompleted: {
        append({commentData: {
                       userId: 1,
                       created: qsTrId("orn-just-now"),
                       commentId: 1,
                       //: https://simple.wikipedia.org/wiki/42_(answer)
                       //% "Deep Thought"
                       userName: qsTrId("orn-dcm-user2"),
                       text: "42",
                       parentId: 0
                   }})
        append({commentData: {
                       userId: 0,
                       //% "7.5 million years ago"
                       created: qsTrId("orn-hint-commentdelegate-created"),
                       commentId: 0,
                       //: https://simple.wikipedia.org/wiki/42_(answer)
                       //% "A little white mice"
                       userName: qsTrId("orn-dcm-user1"),
                       //: https://simple.wikipedia.org/wiki/42_(answer)
                       //% "What is the Answer to the Ultimate Question of Life, the Universe, and Everything?"
                       text: qsTrId("orn-dcm-question")
                   }})
    }
}
