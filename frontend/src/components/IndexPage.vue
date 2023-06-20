<template>
  <section class="text-center py-4 py-xl-5">
    <h4 class="fw-bold">Войдите в свой аккаунт для регистрации устройства</h4>
    <div class="container">
      <div class="row d-flex justify-content-center">
        <div class="col-md-6 col-xl-4">
          <div class="card mb-5">
            <div class="card-body d-flex flex-column align-items-center">
              <form @submit.prevent="GetLogin" class="text-center" method="post">
                <div class="mb-3"><input v-model="login.email" class="form-control" type="email" name="email" placeholder="Email" /></div>
                <div class="mb-3"><input v-model="login.password" class="form-control" type="password" name="password" placeholder="Пароль" /></div>
                <div class="mb-3"><button class="btn btn-primary d-block w-100" type="submit">Войти</button></div>
                <p v-if="error400_flag" class="text-danger">email или пароль неверный</p>
                <p v-if="error500_flag" class="text-danger">ошибка сервера</p>
              </form>
            </div>
          </div>
        </div>
      </div>
    </div>
  </section>
</template>

<script>
import axios from "axios";
export default {
  name:"IndexPage",
  data(){
    return{
      login:{
        email:'',
        password:''
      },
      error400_flag:false,
      error500_flag:false,
    }
  },
  methods:{
    GetLogin(){
      axios.post('login/',this.login)
          .then((response) => {
            localStorage.setItem('access',response.data.data.access)
            localStorage.setItem('refresh',response.data.data.refresh)
            console.log(response.data.access)

            this.$router.push('/reg')


          })
          .catch((error) => {
            let status = error.response.status
            console.log(error)
            status = status - status % 100
            if(status  === 400){
              this.error400_flag = true
              this.error500_flag = false
            }
            if(status === 500){
              this.error400_flag = false
              this.error500_flag = true
            }

          })
    }
  }
}
</script>

<style scoped>

</style>