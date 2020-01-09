如果属性过多，使用setter方法代码重复度过高

# 简单Java类属性自动设置解决方案
1. 下面采用 属性:内容|属性:内容 的形式来为简单Java类的属性初始化
2. 类设计的基本结构：应该由一个专门的 ClassIntanceFactory 类负责所有的反射处理，即，接收反射对象与要设置的属性内容，同时可以获取指定类的实例化对象

## 单极属性配置
在类里面所给出的数据类型都没有其他引用关联，这样的设置称为单极属性配置
1. 需要通过反射进行指定类对象的实例化处理
2. 进行内容的设置（Field属性类型、方法名称、要设置的内容）

```java
package cn.mldn.demo;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

class Emp {
	private String ename;
	private String job;
	public String getEname() {
		return ename;
	}
	public void setEname(String ename) {
		this.ename = ename;
	}
	public String getJob() {
		return job;
	}
	public void setJob(String job) {
		this.job = job;
	}
	public String toString() {
		return "姓名：" + ename + "、 职位: " + job;
	}
}

//首字母大写
class StringUtils {
	public static String initcap(String str) {
		if (str == null || "".equals(str)) return null;
		if (str.length() == 1) return str.toUpperCase();
		return str.substring(0, 1).toUpperCase() + str.substring(1);
	}
}

class BeanUtils {
	private BeanUtils() {}
	
	public static void setValue(Object obj, String val) {
		String [] results = val.split("\\|");
		for (String result : results) {
			String [] attval = result.split(":");
			try {
				Field field = obj.getClass().getDeclaredField(attval[0]);
				Method setMethod = obj.getClass().getDeclaredMethod("set" + StringUtils.initcap(attval[0]), field.getType());
				setMethod.invoke(obj, attval[1]);
			} catch (Exception e) {
				e.printStackTrace();
			}
			
		}
	}
}

class ClassInstanceFactory {
	private ClassInstanceFactory() {}
	
	@SuppressWarnings("unchecked")
	public static <T> T create(Class<?> clazz, String value) {
		//如果想要采用反射进行简单Java类对象属性设置的时候，类中必须要有无参构造
		try {
			Object obj = clazz.getDeclaredConstructor().newInstance();
			BeanUtils.setValue(obj, value);
			return (T) obj;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}
}

public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		String value = "ename:Smith|job:Clerk";
		Emp empObject = ClassInstanceFactory.create(Emp.class, value);
		System.out.println(empObject);
	}
	
}

```

上述的属性类型只能为 String，以下要求实现不同类型的内容的设置

## 设置多种属性类型
```java
package cn.mldn.demo;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

class Emp {
	private long empno;
	private String ename;
	private String job;
	private double salary;
	private Date hiredate;
	public String getEname() {
		return ename;
	}
	public void setEname(String ename) {
		this.ename = ename;
	}
	public String getJob() {
		return job;
	}
	public void setJob(String job) {
		this.job = job;
	}
	public String toString() {
		return "姓名：" + ename + "、 职位: " + job + "、 工号: " + empno + "、 薪水: " + salary + "、 入职日期: " + hiredate;
	}
	public double getSalary() {
		return salary;
	}
	public void setSalary(double salary) {
		this.salary = salary;
	}
	public Date getHiredate() {
		return hiredate;
	}
	public void setHiredate(Date hiredate) {
		this.hiredate = hiredate;
	}
	public long getEmpno() {
		return empno;
	}
	public void setEmpno(long empno) {
		this.empno = empno;
	}
}

//首字母大写
class StringUtils {
	public static String initcap(String str) {
		if (str == null || "".equals(str)) return null;
		if (str.length() == 1) return str.toUpperCase();
		return str.substring(0, 1).toUpperCase() + str.substring(1);
	}
}

class BeanUtils {
	private BeanUtils() {}
	
	public static void setValue(Object obj, String val) {
		String [] results = val.split("\\|");
		for (String result : results) {
			String [] attval = result.split(":");
			try {
				Field field = obj.getClass().getDeclaredField(attval[0]);
				Method setMethod = obj.getClass().getDeclaredMethod("set" + StringUtils.initcap(attval[0]), field.getType());
				Object convertValue = convertAttributeValue(field.getType().getName(), attval[1]);
				setMethod.invoke(obj, convertValue);
			} catch (Exception e) {
				e.printStackTrace();
			}
			
		}
	}
	
	//实现属性类型转换处理
	private static Object convertAttributeValue(String type, String value) {
		if ("long".equals(type) || "java.lang.Long".equals(type)) {
			return Long.parseLong(value);
		}
		if ("int".equals(type) || "java.lang.int".equals(type)) {
			return Integer.parseInt(value);
		}
		if ("double".equals(type) || "java.lang.double".equals(type)) {
			return Double.parseDouble(value);
		}
		if ("java.util.Date".equals(type)) {
			SimpleDateFormat sdf = null;
			if (value.matches("\\d{4}-\\d{2}-\\d{2}")) {
				sdf = new SimpleDateFormat("yyyy-MM-dd");
			} else if (value.matches("\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}")) {
				sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
			} else {
				return new Date();
			}
			try {
				return sdf.parse(value);
			} catch (ParseException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return new Date();
			}
		} else {
			return value;
		}
	}
}

class ClassInstanceFactory {
	private ClassInstanceFactory() {}
	
	@SuppressWarnings("unchecked")
	public static <T> T create(Class<?> clazz, String value) {
		//如果想要采用反射进行简单Java类对象属性设置的时候，类中必须要有无参构造
		try {
			Object obj = clazz.getDeclaredConstructor().newInstance();
			BeanUtils.setValue(obj, value);
			return (T) obj;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}
}

public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		String value = "empno:7369|ename:Smith|job:Clerk|salary:750|hiredate:1989-10-10";
		Emp empObject = ClassInstanceFactory.create(Emp.class, value);
		System.out.println(empObject);
	}
	
}

```


# 级联对象实例化
```java
package cn.mldn.demo;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;

class Company {
	private String name;
	private Date createDate;
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public Date getCreateDate() {
		return createDate;
	}
	public void setCreateDate(Date createDate) {
		this.createDate = createDate;
	}
	public String toString() {
		return "公司: " + name + "、 创建日期: " + createDate;
	}
}

class Dept {
	private String dname;
	private String loc;
	private Company company;
	public String getDname() {
		return dname;
	}
	public void setDname(String dname) {
		this.dname = dname;
	}
	public String getLoc() {
		return loc;
	}
	public void setLoc(String loc) {
		this.loc = loc;
	}
	public Company getCompany() {
		return company;
	}
	public void setCompany(Company company) {
		this.company = company;
	}
	
	public String toString() {
		return "部门: " + dname + "、 loc: " + loc + " " + company.toString();
	}
}

class Emp {
	private long empno;
	private String ename;
	private String job;
	private double salary;
	private Date hiredate;
	private Dept dept;
	public String getEname() {
		return ename;
	}
	public void setEname(String ename) {
		this.ename = ename;
	}
	public String getJob() {
		return job;
	}
	public void setJob(String job) {
		this.job = job;
	}
	public String toString() {
		return "姓名：" + ename + "、 职位: " + job + "、 工号: " + empno + "、 薪水: " + salary + "、 入职日期: " 
				+ hiredate + "  " + dept.toString();
	}
	public double getSalary() {
		return salary;
	}
	public void setSalary(double salary) {
		this.salary = salary;
	}
	public Date getHiredate() {
		return hiredate;
	}
	public void setHiredate(Date hiredate) {
		this.hiredate = hiredate;
	}
	public long getEmpno() {
		return empno;
	}
	public void setEmpno(long empno) {
		this.empno = empno;
	}
	public Dept getDept() {
		return dept;
	}
	public void setDept(Dept dept) {
		this.dept = dept;
	}
}

//首字母大写
class StringUtils {
	public static String initcap(String str) {
		if (str == null || "".equals(str)) return null;
		if (str.length() == 1) return str.toUpperCase();
		return str.substring(0, 1).toUpperCase() + str.substring(1);
	}
}

class BeanUtils {
	private BeanUtils() {}
	
	public static void setValue(Object obj, String val) {
		String [] results = val.split("\\|");
		for (String result : results) {
			String [] attval = result.split(":");
			try {
				String [] clazzes = attval[0].split("\\.");
				Object temp_obj = obj; 
				for (int i = 0; i < clazzes.length - 1; i++) {
					Method getMethod = temp_obj.getClass().getDeclaredMethod("get" + StringUtils.initcap(clazzes[i]));
					Object new_obj = getMethod.invoke(temp_obj);
					if (new_obj == null) {
						Field field = temp_obj.getClass().getDeclaredField(clazzes[i]);
						new_obj = field.getType().getDeclaredConstructor().newInstance();
					}
					Method setMethod = temp_obj.getClass().getDeclaredMethod("set" + StringUtils.initcap(clazzes[i]), new_obj.getClass());
					setMethod.invoke(temp_obj, new_obj);
					temp_obj = new_obj;
				}
				String attribute = clazzes[clazzes.length - 1];
				Field field = temp_obj.getClass().getDeclaredField(attribute);
				Method setMethod = temp_obj.getClass().getDeclaredMethod("set" + StringUtils.initcap(attribute), field.getType());
				Object convertValue = convertAttributeValue(field.getType().getName(), attval[1]);
				setMethod.invoke(temp_obj, convertValue);

			} catch (Exception e) {
				e.printStackTrace();
			}
			
		}
	}
	
	//实现属性类型转换处理
	private static Object convertAttributeValue(String type, String value) {
		if ("long".equals(type) || "java.lang.Long".equals(type)) {
			return Long.parseLong(value);
		}
		if ("int".equals(type) || "java.lang.int".equals(type)) {
			return Integer.parseInt(value);
		}
		if ("double".equals(type) || "java.lang.double".equals(type)) {
			return Double.parseDouble(value);
		}
		if ("java.util.Date".equals(type)) {
			SimpleDateFormat sdf = null;
			if (value.matches("\\d{4}-\\d{2}-\\d{2}")) {
				sdf = new SimpleDateFormat("yyyy-MM-dd");
			} else if (value.matches("\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}")) {
				sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
			} else {
				return new Date();
			}
			try {
				return sdf.parse(value);
			} catch (ParseException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return new Date();
			}
		} else {
			return value;
		}
	}
}

class ClassInstanceFactory {
	private ClassInstanceFactory() {}
	
	@SuppressWarnings("unchecked")
	public static <T> T create(Class<?> clazz, String value) {
		//如果想要采用反射进行简单Java类对象属性设置的时候，类中必须要有无参构造
		try {
			Object obj = clazz.getDeclaredConstructor().newInstance();
			BeanUtils.setValue(obj, value);
			return (T) obj;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}
}

public class JavaAPIDemo {
	
	public static void main(String[] args) throws Exception {
		String value = "empno:7369|ename:Smith|job:Clerk|salary:750|hiredate:1989-10-10|" +
				"dept.dname:财务部|dept.company.name:MLDN";
		Emp empObject = ClassInstanceFactory.create(Emp.class, value);
		System.out.println(empObject);
	}
	
}

```