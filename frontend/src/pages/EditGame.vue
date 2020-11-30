<script>
export default {
    name: 'EditGame',
    data: () => ({
        gameRefPlayers: [
            {name: 'Naive Player', id: '1'},
            {name: 'Smart Player', id: '2'},
        ],
    }),
}

</script>

<template lang="pug">
div
    h2 Create Game

    h3 Basic Information
    .hflex.flex-wrap.hlist-1
        div
            h5 Name
            input(type='text' placeholder='Game name')
        div
            h5 Subtitle
            input(type='text' placeholder='Game subtitle')
        div
            h5 Deadline
            input(type='text' placeholder='2021-01-28')

    h3 Game Resources
    div
        div
            h5 Overview file (markdown)
            label.input-file
                input(type='file')
                span Upload
        div
            h5 Rule file (markdown)
            label.input-file
                input(type='file')
                span Upload
        div
            h5 Game widget (html)
            label.input-file
                input(type='file')
                span Upload

    h3 Game Engine
    .submit-fields.hflex.flex-wrap.hlist-6
        .vflex
            h4 Engine code
            label.input-file
                input(type='file')
                span Upload

        .vflex
            h4 Environment
            .select
                select
                    option(v-for="env in $s.envs") {{ env.name }}
                label

        .vflex
            h4 Execution
            .hflex.hlist-3
                label.input-radio
                    input(type='radio' name='gofirst')
                    span Auto
                label.input-radio
                    input(type='radio' name='gofirst')
                    span Makefile


    h3 Reference Players
    table
        tr
            th Name
            th Actions
        tr(v-for="player in gameRefPlayers" :key="player.id")
            td {{ player.name }}
            td.hcombo
                button Rename
                button Delete
        tr
            td New player
            td
                button Create
</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"
</style>

