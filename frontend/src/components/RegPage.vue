<template>
  <h2 v-if="success" class="text-center" style="margin-bottom: 12px;">Устройство успешно настроено, требуется перезагрузка</h2>
  <section v-if="!success" class="text-center py-4 py-xl-5">
    <h2 class="text-center" style="margin-bottom: 12px;">Введите название устройства</h2>
    <div class="container">
      <div class="row d-flex justify-content-center">
        <div class="col-md-6 col-xl-4">
          <div class="card mb-5">
            <input v-model="devicename" class="form-control">
          </div>
        </div>
      </div>
    </div>
    <button @click="regDevice" class="btn btn-success" type="button" style="margin-right: 10px;">Зарегистрировать</button>
    <button @click="decline" class="btn btn-danger" type="button">Отмена</button>
  </section>
</template>

<script>
import axios from "axios";
export default {
  name:"RegPage",
  data(){
    return{
      devicename:'',
      success:false,
      host: location.host
    }
  },
  methods:{
    decline() {
      this.$router.push('/')
    },
    regDevice() {
      let headers = {'Authorization': "Bearer " + localStorage.getItem('access')}
      let body = {
        'name':this.devicename,
        'status':false
      }
      axios.post('buttons/', body ,{headers})
          .then((response) => {
            let bodyForm = new FormData();
            console.log(this.host)
            bodyForm.append('token',response.data.data.token)
            axios.post('http://'+this.host+'/setToken',bodyForm).then((response1) => {
              console.log(response1)
              this.success = true
            }).catch((error) => {
              console.log(error)
            })
          })
          .catch((error) => {
              console.log(error)
          })
    }
  }
}
</script>

<style scoped>

</style>