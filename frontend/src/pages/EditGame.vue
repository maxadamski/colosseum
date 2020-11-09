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
    .hflex.hlist
        div
            span.left Name
            input(type='text' placeholder='Game name')
        div
            span.left Subtitle
            input(type='text' placeholder='Game subtitle')
        div
            span.left Deadline
            input(type='text' placeholder='2021-01-28')

    h3 Game Resources
    div
        span.left Overview file (markdown)
        button Upload
    br
    div
        span.left Rule file (markdown)
        button Upload

    h3 Game Engine
    .submit-fields.hflex
        .vflex
            span Engine code
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


    h3 Reference Players
    table.w-50
        tr
            th Name
            th Actions
        tr
            td Naive Player
            td.hlist
                button Rename
                button Delete
        tr
            td Smart Player
            td.hlist
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
