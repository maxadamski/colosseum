<script>
import {now, datediff} from '../common'

export default {
    name: 'Index',
    data: () => ({
        tab: 'overview',
        studentTabs: ['overview', 'rules', 'play', 'team', 'submit'],
        teacherTabs: ['overview', 'rules', 'play'],
        player: 'Naive Player',
        submitEnv: '1',
        isAutomake: true,
        firstPlayer: 'you',
        invitedStudent: '',
        submissionFile: null
    }),
    computed: {
        deadlineCountdown() {
            const dt = datediff(now(), new Date(this.$s.game.deadline))
            let res = ''
            const plural = (x) => x == 1 ? '' : 's'
            if (dt.months > 0) res += `${dt.months} month${plural(dt.months)}`
            if (dt.days > 0) res += ` ${dt.days} day` + plural(dt.days)
            if (!res) res += `${dt.hours} hour${plural(dt.hours)} and ${dt.minutes} minute${plural(dt.minutes)}`
            return res
        },
        isLeader() {
            return this.$s.studentId === this.$s.teamLeaderId
        }
    },
    methods: {
        async changeTeamName() {
            const [teamPatch, teamPatchStatus] = await this.safeApi('PATCH', `/teams/${this.$s.teamId}`, JSON.stringify({new_name: this.$s.teamName}))
            if (teamPatchStatus === 403) {
                console.log(`You are not the leader! (status code ${studentTeamStatus})`)
                const [studentTeam, studentTeamStatus] = await this.safeApi('GET', '/students/me/team')
                this.$s.teamName = studentTeam.name
            } else if (teamPatchStatus === 422) {
                console.log(`Team name cannot be empty or have more than 50 characters (status code ${studentTeamStatus})`)
                const [studentTeam, studentTeamStatus] = await this.safeApi('GET', '/students/me/team')
                this.$s.teamName = studentTeam.name
            }
        },

        async acceptTeamInvitation(invitationId, index) {
            await this.safeApi('POST', `/students/me/invitations/${invitationId}/accept`)
            this.$delete(this.$s.studentInvitations, index)

            const [studentTeam, studentTeamStatus] = await this.safeApi('GET', '/students/me/team')
            this.$s.teamId = studentTeam.id
            this.$s.teamName = studentTeam.name
            this.$s.teamLeaderId = studentTeam.leader_id

            const [studentTeamMembers, teamMembersStatus] = await this.safeApi('GET', `/team/${this.$s.teamId}/members`)
            this.$s.teamMembers = studentTeamMembers

            const [studentTeamInvitations, teamInvitationsStatus] = await this.safeApi('GET', `/team/${this.$s.teamId}/invitations`)
            this.$s.teamInvitations = studentTeamInvitations

            const [teamSubmissions, teamSubmissionsStatus] = await this.safeApi('GET', `/teams/${this.$s.teamId}/submissions`)
            this.$s.teamSubmissions = teamSubmissions
        },

        async inviteStudent() {
            await this.safeApi('POST', `/teams/${this.$s.teamId}/invitations/${this.invitedStudent}`)
            const [studentTeamInvitations, teamInvitationsStatus] = await this.safeApi('GET', `/team/${this.$s.teamId}/invitations`)
            if (teamInvitationsStatus === 403) {
                console.log(`You are not the leader! (status code ${teamInvitationsStatus})`)
            } else if (teamInvitationsStatus === 404) {
                console.log(`No such student! (status code ${teamInvitationsStatus})`)
            } else {
                this.$s.teamInvitations = studentTeamInvitations
            }
        },

        submissionOnFileChanged(event) {
            this.submissionFile = event.target.files[0]
        },
        async sendSubmission() {
            const submissionForm = new FormData()
            submissionForm.append('executables', this.submissionFile)
            submissionForm.append('environment_id', this.submitEnv)
            submissionForm.append('is_automake', this.isAutomake)
            const [submissionPost, submissionPostStatus] = await this.safeApi('POST', `/teams/me/submissions`, submissionForm)
            if (submissionPostStatus === 415) {
                console.log(`Bad extension! (status conde ${submissionPostStatus})`)
            } else if (submissionPostStatus === 413) {
                console.log(`Bad size! (status conde ${submissionPostStatus})`)
            } else {
                const [teamSubmissions, teamSubmissionsStatus] = await this.safeApi('GET', `/teams/${this.$s.teamId}/submissions`)
                this.$s.teamSubmissions = teamSubmissions
            }
        },
        indexWhere(array, conditionFn) {
            const item = array.find(conditionFn)
            return array.indexOf(item)
        },
        async changePrimarySubmission(subId, index) {
            await this.safeApi('POST', `/teams/me/submissions/primary/${subId}`)
            let prevPrimary = this.indexWhere(this.$s.teamSubmissions, sub => sub.primary === true)
            this.$s.teamSubmissions[prevPrimary].primary = false
            this.$s.teamSubmissions[index].primary = true
        }
    }
}
</script>

<template lang="pug">
    div
        h1.mb-0 {{ $s.game.name }}
        h4 {{ $s.game.description }}
        b {{ deadlineCountdown }} left

        nav.tabs(v-if='$s.userType == "teacher"')
            button(v-for='x in teacherTabs' @click='tab = x' :class='{selected: tab == x}') {{x}}
        nav.tabs(v-else)
            button(v-for='x in studentTabs' @click='tab = x' :class='{selected: tab == x}') {{x}}

        div(v-if='tab == "overview"')
            div(v-html='$s.game.overview')

        div(v-if='tab == "rules"')
            div(v-html='$s.game.rules')

        div(v-if='tab == "play"')
            h3 Interactive Game

            .hflex.hlist-6
                .vflex
                    h4 Player
                    .select
                        select(v-model='player')
                            option(v-for='x in $s.refPlayers') {{x.name}}
                .vflex
                    h4 First player
                    .hflex.hlist-3
                        label.input-radio
                            input(type='radio' v-model='firstPlayer' value='you')
                            span You
                        label.input-radio
                            input(type='radio' v-model='firstPlayer' value='them')
                            span Them

            iframe.widget(:src='$s.game.widget')


        div(v-if='tab == "team"')
            h3 Team

            h4(v-if="isLeader") Team Name
                .hcombo
                    input(type='text' v-model='$s.teamName')
                    button(@click="changeTeamName") Save

            h4 Team Members
            table
                tr
                    th User
                    th Status
                    th(v-if="isLeader") Actions

                tr(v-for="member in $s.teamMembers" :key="`TM-${member.id}`")
                    td {{ member.nickname}}

                    td(v-if="member.id === $s.teamLeaderId") Leader
                    td(v-else) Member

                    td(v-if="isLeader && member.id !== $s.studentId ")
                        button(@click="safeApi('PATCH', `/teams/${$s.teamId}/leader/${member.id}`);$s.teamLeaderId = member.id") Set Leader
                tr(v-for="(invited, index) in $s.teamInvitations" :key="`TI-${invited.id}`")
                    td {{ invited.nickname}}

                    td Invited

                    td(v-if="isLeader")
                        button(@click="safeApi('DELETE', `/students/${$s.teamId}/invitations/${invited.id}`); $delete($s.teamInvitations, index)") Cancel Invite

            h3 Invitations
            .hflex.hlist-3.fy-center(v-if="($s.studentInvitations).length")
                span(v-for="(invitation, index) in $s.studentInvitations" :key="invitation.id")
                    b {{ invitation.leader }}
                    span  invited you to team 
                    b {{ invitation.name }}
                    .hcombo
                        button(@click="acceptTeamInvitation(invitation.id, index)") Accept
                        button(@click="safeApi('POST', `/students/me/invitations/${invitation.id}/decline`); $delete($s.studentInvitations, index)") Delete
            p(v-else) There are no invitations 

            h4(v-if="isLeader") Invite Member
                .hcombo
                    input(type='text' placeholder='Student nickname', v-model="invitedStudent")
                    button(@click="inviteStudent()") Send

        div(v-if='tab == "submit"')
            h3 New Submission

            .hflex.hlist-6
                .vflex
                    h4 Player code
                    label.input-file
                        input(type='file' @change="submissionOnFileChanged")
                        | Upload

                .vflex
                    h4 Environment
                    .select
                        select(v-model='submitEnv')
                            option(v-for='env in $s.envs' :value='env.id') {{env.name}}

                .vflex
                    h4 Execution
                    .hflex.hlist-3
                        label.input-radio
                            input(type='radio' v-model='isAutomake' value="true")
                            span Auto
                        label.input-radio
                            input(type='radio' v-model='isAutomake' value="false")
                            span Makefile

                .vflex
                    h4 Actions
                    .hflex.hlist-1
                        button(@click="sendSubmission()") Submit


            h3 My Submissions
            table.submission-table
                tr
                    th #
                    th Date
                    th Env
                    th State
                    th Score
                    th(v-if="isLeader") Actions

                tr(v-for='(sub, index) in $s.teamSubmissions' :key='sub.id')
                    td
                        span(v-if='sub.primary === true')
                            b.tooltip(title="This is your team's primary submission. Your final grade will depend on the performance of this submission") {{sub.id}}*
                        span(v-else) {{ sub.id }}
                    td {{ sub.date }}
                    td {{ sub.env }}
                    td {{ sub.status }}
                    td {{ sub.score }}
                    td(v-if="isLeader")
                        button(v-if='!sub.primary' @click="changePrimarySubmission(sub.id, index)") Set Primary
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
        border none
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

.submission-table > tr > :nth-child(2)
    width 20ch
</style>

