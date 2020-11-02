<script>
export default {
    name: 'App',
    data: () => ({
        tabs: [
            {name: 'Home', url: '/'},
            {name: 'Profile', url: '/profile'},
        ],
    }),
}
</script>

<template lang="pug">
#inner-app
    header
        router-link(to='/')
            img(src='../static/logo.png')
        nav
            router-link(v-for='tab in tabs' :to='tab.url') {{tab.name}}
    main
        router-view
    footer
        span [Logo left]
        small &copy; Copyright 2020
        span [Logo right]
</template>

<style lang="stylus">
@import "../styles/basic.styl"
</style>

<style lang="stylus" scoped>
@import "../styles/shared.styl"

#inner-app
    display grid
    grid-template auto 1fr auto / 1fr
    height 100vh
    max-width 1024px
    margin 0 auto
    padding 32px 10px

nav
    hgap 20px

header
    hflex space-between center

footer
    hflex space-between center
    height 40px

header img
    height 64px
</style>