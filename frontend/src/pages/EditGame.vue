<script>
export default {
    name: 'EditGame',
    data: () => ({
    }),
}
</script>

<template lang="pug">
div
    h2 Create Game

    h3 Basic Information
    .hflex.hlist-1
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
            div
                input(type='file' id='overview-rules')
                label(for='overview-rules') Upload
        div
            h5 Rule file (markdown)
            div
                input(type='file' id='doc-rules')
                label(for='doc-rules') Upload
        div
            h5 Game widget (html)
            div
                input(type='file' id='game-widget')
                label(for='game-widget') Upload

    h3 Game Engine
    .submit-fields.hflex.hlist-6
        .vflex
            h4 Engine code
            div
                input(type='file' id='engine-code')
                label(for='engine-code') Upload

        .vflex
            h4 Environment
            .select
                select
                    option Python3
                    option C++
                label

        .vflex
            h4 Execution
            .hflex.hlist-3
                label.checkbox
                    input(type='radio' name='gofirst')
                    span Auto
                label.checkbox
                    input(type='radio' name='gofirst')
                    span Makefile


    h3 Reference Players
    .rounded.w-50
        table.w-100
            tr
                th Name
                th Actions
            tr
                td Naive Player
                td.hcombo
                    button Rename
                    button Delete
            tr
                td Smart Player
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

.left
    display block
</style>
