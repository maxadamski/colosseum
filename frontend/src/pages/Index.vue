<script>
export default {
    name: 'Index',
    data: () => ({
        tabs: ['overview', 'rules', 'play', 'team', 'submit'],
        tab: 'overview',
        players: ['Naive Player', 'Smart Player', 'Tricky Player'],
        player: 'Naive Player',
        teamName: 'Foobar',
        game: 'Pentago',
        timeleft: '2 months'
    }),
}
</script>

<template lang="pug">
div
    h1.mb-0 {{ game }}
    i Win in a two-player match 
    br
    span time left 
        b {{timeleft }}
    nav.tabs
        button(v-for='x in tabs' @click='tab = x' :class='{selected: tab == x}') {{x}}

    div(v-if='tab == "overview"')
        h2 Overview of the game
        p A rendered markdown document

    div(v-if='tab == "rules"')
        h2 Rules of the game
        p A rendered markdown document

    div(v-if='tab == "play"')
        h3 Interactive Game

        .submit-fields.hflex
            .vflex
                span Player
                .select
                    select
                        option(v-for='x in players' :selected='player == x') {{x}}
            .vflex
                span First player
                .hflex.hlist
                    label
                        input(type='radio' name='gofirst')
                        |You
                    label
                        input(type='radio' name='gofirst')
                        |Them
        .widget Custom Game Widget


    div(v-if='tab == "team"')
        h3 Team Name
        .hlist
            input(type='text' :value='teamName')
            button Save

        h3 Team Members
        table.w-50
            tr
                th User
                th Status
                th Actions
            tr
                td Max
                td Leader
                td
            tr
                td Piotr
                td Member
                td
                    button Set Leader
            tr
                td Sławek
                td Invited
                td
                    button Cancel Invite

        h3 Send Invitation
        .hlist
            input(type='text' placeholder='User nickname')
            button Send

        h3 Received Invitations
        .hlist
            span Team <b>Fizzbuzz</b> with leader <b>Maciej</b>
            button Accept

    div(v-if='tab == "submit"')
        h3 New Submission

        .submit-fields.hflex
            .vflex
                span Player code
                .hflex
                    button Upload

            .vflex
                span Environment
                .select
                    select
                        option Python3
                        option C++

            .vflex
                span Execution
                .hflex.hlist
                    label
                        input(type='radio' name='gofirst')
                        |Auto
                    label
                        input(type='radio' name='gofirst')
                        |Makefile

            .vflex
                span Actions
                .hflex.hlist
                    button Submit
                    button Cancel

        h3 My Submissions
        table.w-100
            tr
                th #
                th Date
                th Env
                th State
                th Score
            tr
                td 1
                td 2020-10-11 10:15
                td Python3
                td ok
                td 80%
            tr
                td 2
                td 2020-10-11 10:20
                td C++
                td bulid failed
                td n/a
            tr
                td 2
                td 2020-10-10 23:12
                td C++
                td ok
                td 78%



</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"

.tabs
    display flex
    width 100%
    margin 20px 0

    button
        all unset
        flex-basis 100%
        text-align center
        border-bottom 1px solid decor-color
        height 32px
        font-size 0.8em
        background white
        cursor pointer
        text-transform uppercase

    button.selected
        border 1px solid decor-color
        border-bottom none

    button:hover
        background #eee

.widget
    background gray
    width 100%
    height 500px
    hflex center center
</style>
