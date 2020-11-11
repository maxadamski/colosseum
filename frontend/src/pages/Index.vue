<script>
export default {
    name: 'Index',
    data: () => ({
        tabs: ['overview', 'rules', 'play', 'team', 'submit'],
        tab: 'submit',
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
    h4 Win in a two-player match
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

        .hflex.hlist-6
            .vflex
                h4 Player
                .select
                    select
                        option(v-for='x in players' :selected='player == x') {{x}}
            .vflex
                h4 First player
                .hflex.hlist-3
                    label.checkbox
                        input(type='radio' name='gofirst')
                        span You
                    label.checkbox
                        input(type='radio' name='gofirst')
                        span Them
        .widget Custom Game Widget


    div(v-if='tab == "team"')
        h3 Team

        h4 Team Name
        .hcombo
            input(type='text' :value='teamName')
            button Save

        h4 Team Members
        .rounded.w-50
            table
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

        h3 Invitations
        .hflex.hlist-3.fy-center
            span <b>Igor</b> invited you to team <b>Fizzbuzz</b>
            .hcombo
                button Accept
                button Delete

        h4 Invite Member
        .hcombo
            input(type='text' placeholder='User nickname')
            button Send

    div(v-if='tab == "submit"')
        h3 New Submission

        .hflex.hlist-6
            .vflex
                h4 Player code
                .hflex
                    button Upload

            .vflex
                h4 Environment
                .select
                    select
                        option Python3
                        option C++

            .vflex
                h4 Execution
                .hflex.hlist-3
                    label.checkbox
                        input(type='radio' name='gofirst')
                        |Auto
                    label.checkbox
                        input(type='radio' name='gofirst')
                        |Makefile

            .vflex
                h4 Actions
                .hflex.hlist-1
                    button Submit
                    button Cancel


        h3 My Submissions
        table
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

    button:active
        background #ddd

.widget
    background gray
    width 100%
    height 500px
    margin-top u4
    hflex center center
</style>
