<script>
export default {
    name: 'Help',
    data: () => ({
    }),
}
</script>

<template lang="pug">
div.wrap
    h1 Heading 1
    h2 Heading 2
    h3 Heading 3
    h4 Heading 4
    h5 Heading 5

    h1 Hello, world

    p Dolor sapiente excepturi voluptatem facilis earum fugiat. A aspernatur fugit molestiae eum quos. Dolor non possimus dicta hic. Numquam vero qui ducimus labore ut.

    p Rerum vitae velit et fugit maxime. Id ab ut similique. Quasi nostrum unde et. Aut sed omnis voluptate laboriosam ut. Distinctio sed officiis quidem vero quia a. Odit sunt vitae molestias id sunt temporibus quis est.

    .vlist
        .hlist
            button Accept
            button Cancel

        .hlist
            input(type='text')
            button Save

        input(type='file' id='xx')
        label(for='xx') Upload


</template>

<style lang="stylus" scoped>
@import "../styles/shared.styl"

.wrap
    border 1px solid black
</style>
