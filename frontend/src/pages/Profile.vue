<script>
export default {
    name: 'Profile',
    data: () => ({
        willDeleteAccount: false,
        deleteAccountConfirm: undefined,
        newGroup: null,
        editGroup: null,
    }),
    methods: {
        addGroup(name) {
            this.$s.groups.push({id: -1, name: name})
        },
        updateGroup(group) {
            for (const i in this.$s.groups) {
                if (this.$s.groups[i].id == group.id) {
                    this.$s.groups[i] = group
                    break
                }
            }
        },
        async activateGame(gameId) {
            await this.safeApi('POST', `/games/activate/${gameId}`)
            const [groupsData, groupsStatus] = await this.safeApi('GET', `/groups`)
            this.$s.groups = groupsData

            const [envsData, envsStatus] = await this.safeApi('GET', `/environments`)
            this.$s.envs = envsData

            const [gameData, gameStatus] = await this.safeApi('GET', '/games/active')
            if (gameStatus != 200) {
                console.log(`No active game! (status code ${gameStatus})`)
            } else {
                this.$s.game = gameData
                const [gameWidget, gameWidgetStatus] = await this.safeApi('GET', `/games/${gameData.id}/widget`)
                this.$s.game.widget = gameWidget.html
                const [refPlayers, refPlayersStatus] = await this.safeApi('GET', `/games/${gameData.id}/ref_submissions`)
                this.$s.refPlayers = refPlayers
            }
            const [gamesData, gamesStatus] = await this.safeApi('GET', `/games`)
            this.$s.games = gamesData
        },
        async changeStudentNickname() {
            const [teamPatch, teamPatchStatus] = await this.safeApi('PATCH', `/students/me`, JSON.stringify({nickname: this.$s.studentNick}))
            if (teamPatchStatus === 422) {
                console.log(`Nickname cannot be empty or have more than 50 characters (status code ${teamPatchStatus})`)
                const [userData, userStatus] = await this.safeApi('GET', '/students/me')
                this.$s.studentNick = userData.nickname
            }
        },
        async deleteAccount() {
            if (this.deleteAccountConfirm !== 'I want to delete my account') return
            this.willDeleteAccount = false;
            this.logOut()
        },
    }

}
</script>

<template lang="pug">
    div
        h2 User Profile

        .debug
            .hflex.hlist-3
                label.input-radio
                    input(type='radio' v-model='$s.userType' value='student')
                    span Student
                label.input-radio
                    input(type='radio' v-model='$s.userType' value='teacher')
                    span Teacher

            .div
                button(@click='doLogin') Login


        div(v-if='$s.userType == "student"')
            h3 Basic Information

            h4 Nickname
            .hcombo
                input(type='text' v-model='$s.studentNick')
                button(@click="changeStudentNickname") Save

            h4 Group
            .hcombo
                select(v-model='$s.studentGroup')
                    option(disabled :value='null') Click to select
                    option(v-for='group in $s.groups' :value='group.id') {{ group.name }}
                button(@click="safeApi('PATCH', `/students/me`, JSON.stringify({group_id: $s.studentGroup}))") Save

            h3 Account Actions

            h4 Danger Zone


            div(v-if='willDeleteAccount')
                p To delete your account, type "I want to delete my account" below and click "Confirm".

                p <u>This operation will delete all your data and is irreversible!</u>

                .hcombo
                    input(type='text' placeholder='Type here...' v-model='deleteAccountConfirm')
                    button(@click='deleteAccount' :disabled='deleteAccountConfirm !== "I want to delete my account"') Confirm

                p If you don&#39;t want to delete your account, click "Cancel".

                button(@click='deleteAccountConfirm = undefined; willDeleteAccount = false') Cancel

            div(v-else)
                button(@click='willDeleteAccount = true') Delete Account


        div(v-if='$s.userType == "teacher"')
            h2 Game-Maker Zone

            h3 My Games

            table
                tr
                    th Game
                    th Actions

                tr(v-for='(game,index) in $s.games' :key='game.id')
                    td {{ game.name }}
                    td
                        .hcombo.mr-2
                            button(v-if='game.id !== $s.game.id' @click="activateGame(game.id)") Activate
                            button Reset

                        .hcombo
                            router-link(:to="`/game-wizard?edit=${game.id}`")
                                button Edit
                            button(v-if='game.id !== $s.game.id' @click="safeApi('DELETE', `/games/${game.id}`); $delete($s.games, index)") Delete
                tr
                    td New game
                    td.hcombo
                        router-link(to='/game-wizard')
                            button + Create

            h3 My Groups
            table
                tr
                    th Name
                    th Actions
                tr(v-for='(group, index) in $s.groups')
                    template(v-if='editGroup !== null && editGroup.id === group.id')
                        td
                            input(type='text' v-model='editGroup.name')
                        td.hcombo
                            button(@click='updateGroup(editGroup); editGroup = null') Save
                            button(@click='editGroup = null') Cancel

                    template(v-else)
                        td {{ group.name }}
                        td.hcombo
                            button Export
                            button(@click="editGroup = {...group}") Edit
                            button(@click="safeApi('DELETE', `/groups/${group.id}`); $delete($s.groups, index)") Delete

                tr(v-if='newGroup !== null')
                    td
                        input(type='text' v-model='newGroup')
                    td.hcombo
                        button(@click='addGroup(newGroup); newGroup = null') Save
                        button(@click='newGroup = null') Cancel

                tr(v-else)
                    td New group
                    td
                        button(@click='newGroup = "New group"') + Create
</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"

table > tr > :nth-child(1)
    width 20ch
</style>

